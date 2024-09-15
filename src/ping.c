#include "ping.h"
#include "activities.h"  // To check if a process exists in the activities list

void ping_command(pid_t pid, int signal_num) {
    signal_num %= 32;  // Take signal number's modulo with 32
    int found = 0;

    // Check if the process exists in the activities list
    for (int i = 0; i < activities_count; i++) {
        if (activities_list[i].pid == pid) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("No such process found\n");
        return;
    }

    // Try to send the signal to the process
    if (kill(pid, signal_num) == -1) {
        printf("Failed to send signal %d to process with pid %d: %s\n", signal_num, pid, strerror(errno));
    } else {
        printf("Sent signal %d to process with pid %d\n", signal_num, pid);
    }
}
