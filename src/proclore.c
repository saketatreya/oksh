#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libproc.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <sys/proc_info.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include "proclore.h"

void proclore_command(pid_t pid) {
    // If no pid is provided, use the shell's PID
    if (pid == 0) {
        pid = getpid();
    }

    // Print PID
    printf("pid : %d\n", pid);

    // Get process status
    struct kinfo_proc info;
    size_t info_size = sizeof(info);
    int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, pid};
    if (sysctl(mib, 4, &info, &info_size, NULL, 0) != 0) {
        perror("sysctl (process status)");
        return;
    }

    // Process state
    char process_status = info.kp_proc.p_stat;
    char process_state_char = '?';
    char foreground_flag = ' ';
    
    switch (process_status) {
        case SRUN:
            process_state_char = 'R'; // Running
            break;
        case SSLEEP:
            process_state_char = 'S'; // Sleeping
            break;
        case SIDL:
            process_state_char = 'I'; // Idle
            break;
        case SZOMB:
            process_state_char = 'Z'; // Zombie
            break;
        default:
            process_state_char = '?'; // Unknown state
            break;
    }

    // Foreground process flag
    if (info.kp_proc.p_flag & P_CONTROLT) {
        foreground_flag = '+';
    }

    printf("process status : %c%c\n", process_state_char, foreground_flag);

    // Get process group
    pid_t pgid = getpgid(pid);
    printf("Process Group : %d\n", pgid);

    // Get virtual memory size
    struct proc_taskinfo taskinfo;
    if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &taskinfo, sizeof(taskinfo)) == sizeof(taskinfo)) {
        printf("Virtual memory : %llu KB\n", taskinfo.pti_virtual_size / 1024);
    } else {
        perror("proc_pidinfo (virtual memory)");
    }

    // Get the executable path
    char executable_path[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, executable_path, sizeof(executable_path)) > 0) {
        printf("executable path : %s\n", executable_path);
    } else {
        perror("proc_pidpath (executable path)");
    }
}
