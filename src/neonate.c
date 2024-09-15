#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <libproc.h>
#include <sys/proc_info.h>
#include "neonate.h"

// Function to get the most recent PID
pid_t get_most_recent_pid() {
    int mib[3];
    size_t len;
    struct kinfo_proc *procs = NULL;
    size_t proc_count = 0;

    // Fetch all processes using sysctl
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_ALL;

    if (sysctl(mib, 3, NULL, &len, NULL, 0) < 0) {
        perror("sysctl");
        return -1;
    }

    procs = malloc(len);
    if (procs == NULL) {
        perror("malloc");
        return -1;
    }

    if (sysctl(mib, 3, procs, &len, NULL, 0) < 0) {
        perror("sysctl");
        free(procs);
        return -1;
    }

    proc_count = len / sizeof(struct kinfo_proc);

    // Find the most recent process by comparing the process creation time
    pid_t most_recent_pid = 0;
    struct timeval most_recent_time = {0, 0};
    for (size_t i = 0; i < proc_count; i++) {
        struct timeval proc_time = procs[i].kp_proc.p_starttime;
        if (proc_time.tv_sec > most_recent_time.tv_sec ||
            (proc_time.tv_sec == most_recent_time.tv_sec && proc_time.tv_usec > most_recent_time.tv_usec)) {
            most_recent_time = proc_time;
            most_recent_pid = procs[i].kp_proc.p_pid;
        }
    }

    free(procs);
    return most_recent_pid;
}

// Function to check for 'x' key press (non-blocking)
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

// Neonate command implementation
void neonate_command(int interval) {
    while (1) {
        pid_t recent_pid = get_most_recent_pid();
        if (recent_pid != -1) {
            printf("%d\n", recent_pid);
        } else {
            printf("Error fetching recent process ID\n");
        }

        // Wait for 'interval' seconds
        for (int i = 0; i < interval * 10; i++) {
            usleep(100000);  // Sleep in 0.1 second intervals
            if (kbhit()) {
                char key = getchar();
                if (key == 'x' || key == 'X') {
                    printf("Exiting on key press 'x'\n");
                    return;
                }
            }
        }
    }
}
