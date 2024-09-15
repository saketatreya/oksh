#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <errno.h>
#include "activities.h"
#include "fgbg.h"

// Function to bring a background process to the foreground
void fg_command(pid_t pid) {
    int status;
    struct termios shell_tmodes;   // To save the shell's terminal attributes
    pid_t shell_pgid = getpgrp();  // Get the shell's process group ID


    // Search for the process in the activities list
    for (int i = 0; i < activities_count; i++) {
        if (activities_list[i].pid == pid) {

            // Get the process group ID of the target process
            pid_t target_pgid = getpgid(pid);
            if (target_pgid == -1) {
                perror("ERROR: Failed to get process group ID");
                return;
            }

            // Save the shell's terminal attributes
            if (tcgetattr(STDIN_FILENO, &shell_tmodes) == -1) {
                perror("ERROR: Failed to save shell terminal attributes");
                return;
            }

            // If the process is stopped, send SIGCONT to continue it
            if (!activities_list[i].is_running) {
                printf("Process %d is stopped. Bringing it to foreground.\n", pid);
                if (kill(pid, SIGCONT) == -1) {
                    perror("ERROR: Failed to send SIGCONT");
                    return;
                }
            }

            // Set the process group of the target process as the foreground process group
            if (tcsetpgrp(STDIN_FILENO, target_pgid) == -1) {
                perror("ERROR: Failed to set process group to foreground");
                return;
            }

            // Wait for the process to either stop or finish
            if (waitpid(pid, &status, WUNTRACED) == -1) {
                perror("ERROR: Failed during waitpid");
            } else {
            }

            // Restore the terminal control to the shell process group
            if (tcsetpgrp(STDIN_FILENO, shell_pgid) == -1) {
                perror("ERROR: Failed to restore terminal control to shell");
                return;
            }

            // Restore the shell's terminal attributes
            if (tcsetattr(STDIN_FILENO, TCSADRAIN, &shell_tmodes) == -1) {
                perror("ERROR: Failed to restore shell terminal attributes");
                return;
            }

            // Check if the process was stopped
            if (WIFSTOPPED(status)) {
                update_process_state(pid, 0);  // Mark as stopped
            } else {
                update_process_state(pid, 0);  // Mark as finished (no longer running)
            }
            return;
        }
    }

    printf("No such process found.\n");
}

// Function to resume a stopped process in the background
void bg_command(pid_t pid) {
    // Search for the process in the activities list
    for (int i = 0; i < activities_count; i++) {
        if (activities_list[i].pid == pid) {
            if (activities_list[i].is_running) {
                printf("Process %d is already running.\n", pid);
                return;
            }

            printf("Resuming process %d in the background.\n", pid);
            kill(pid, SIGCONT);  // Send SIGCONT to continue the stopped process
            update_process_state(pid, 1);  // Mark as running
            return;
        }
    }

    printf("No such process found.\n");
}
