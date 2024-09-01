#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "shell.h"
#include "command_handler.h"
#include "activities.h"

extern char last_command_name[1024];
extern int last_command_time;

// Global variable to track the foreground process
pid_t foreground_pid = -1;  // Define and initialize here


int main() {
    char home_directory[MAX_PATH_LEN];
    getcwd(home_directory, sizeof(home_directory));


    while (1) {
        display_prompt(home_directory, last_command_name, last_command_time);

        char input[MAX_PATH_LEN];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // If fgets returns NULL, this indicates Ctrl-D (EOF)
            printf("\nLogging out...\n");


            break;  // Exit the shell
        }

        input[strcspn(input, "\n")] = '\0';

        // Parse and execute commands
        parse_and_execute(input, home_directory);

        if (strcmp(input, "exit") == 0) {
            // Kill all running processes before exiting with "exit" command
            break;
        }
    }

    return 0;
}
