#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "shell.h"
#include "command_handler.h"
#include "config.h"
#include "activities.h"

extern char last_command_name[1024];
extern int last_command_time;

// Global variable to track the foreground process
pid_t foreground_pid = -1;  // Define and initialize here

void handle_sigint(int sig) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);  // Send SIGINT to the foreground process
    }
}

void handle_sigtstp(int sig) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGTSTP);  // Send SIGTSTP to the foreground process
    }
}

void handle_sigquit(int sig) {
    printf("\nLogging out...\n");
    exit(0);  // Exit the shell
}

void kill_all_processes() {
    // Kill all processes listed in activities
    for (int i = 0; i < activities_count; i++) {
        if (activities_list[i].is_running) {
            printf("Killing process [%d]: %s\n", activities_list[i].pid, activities_list[i].command_name);
            kill(activities_list[i].pid, SIGKILL);  // Send SIGKILL to all processes
        }
    }
}

int main() {
    char home_directory[MAX_PATH_LEN];
    getcwd(home_directory, sizeof(home_directory));

    // Load .myshrc file
    load_myshrc(home_directory);

    // Set up signal handlers
    signal(SIGINT, handle_sigint);     // Handle Ctrl-C
    signal(SIGTSTP, handle_sigtstp);   // Handle Ctrl-Z
    signal(SIGQUIT, handle_sigquit);   // Handle Ctrl-D (use quit signal)

    while (1) {
        display_prompt(home_directory, last_command_name, last_command_time);

        char input[MAX_PATH_LEN];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // If fgets returns NULL, this indicates Ctrl-D (EOF)
            printf("\nLogging out...\n");

            // Kill all running processes before exiting
            kill_all_processes();

            break;  // Exit the shell
        }

        input[strcspn(input, "\n")] = '\0';

        // Parse and execute commands
        parse_and_execute(input, home_directory);

        if (strcmp(input, "exit") == 0) {
            // Kill all running processes before exiting with "exit" command
            kill_all_processes();
            break;
        }
    }

    return 0;
}
