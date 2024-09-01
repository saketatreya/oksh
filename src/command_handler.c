#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <limits.h>
#include "command_handler.h"
#include "hop.h"
#include "reveal.h"
#include "proclore.h"
#include "seek.h"
#include "log.h"

// Track the last executed command and time
char last_command_name[1024] = "";
int last_command_time = 0;

// External variable to track foreground process
extern pid_t foreground_pid;



void handle_command(char* command, const char* home_directory, int run_in_background) {
    char* args[10];
    int argc = 0;

    char original_command[1024];
    strncpy(original_command, command, sizeof(original_command));
    original_command[sizeof(original_command) - 1] = '\0';

    args[argc] = strtok(command, " ");
    while (args[argc] != NULL && argc < 9) {
        argc++;
        args[argc] = strtok(NULL, " ");
    }

    // Handle log commands
    if (strcmp(args[0], "log") == 0) {
        if (argc == 1) {
            print_log();
        } else if (argc == 2 && strcmp(args[1], "purge") == 0) {
            purge_log();
        } else if (argc == 3 && strcmp(args[1], "execute") == 0) {
            int index = atoi(args[2]);
            execute_from_log(index, home_directory);
        } else {
            printf("Usage: log, log purge, log execute <index>\n");
        }
        return;
    }

    
    // Handle "proclore" command
    if (strcmp(args[0], "proclore") == 0) {
        pid_t pid = 0;
        if (argc > 1) {
            pid = atoi(args[1]);  // Parse provided PID if present
        }
        proclore_command(pid);
        return;
    }


    // Handle "hop" internally without forking
    if (strcmp(args[0], "hop") == 0) {
        hop_command(args + 1, argc - 1, home_directory);
    } else if (strcmp(args[0], "reveal") == 0) {
        int show_all = 0;
        int show_long = 0;
        char* path = ".";

        for (int i = 1; i < argc; i++) {
            if (args[i][0] == '-') {
                for (int j = 1; j < strlen(args[i]); j++) {
                    if (args[i][j] == 'a') show_all = 1;
                    else if (args[i][j] == 'l') show_long = 1;
                }
            } else {
                path = args[i];
            }
        }

        reveal_command(path, show_all, show_long, home_directory);
    } else if (strcmp(args[0], "seek") == 0) {
        // Handle seek command
        int show_dirs = 0, show_files = 0, execute_flag = 0;
        char* target = NULL;
        char* directory = NULL;

        for (int i = 1; i < argc; i++) {
            if (args[i][0] == '-') {
                for (int j = 1; j < strlen(args[i]); j++) {
                    if (args[i][j] == 'd') show_dirs = 1;
                    else if (args[i][j] == 'f') show_files = 1;
                    else if (args[i][j] == 'e') execute_flag = 1;
                }
            } else if (target == NULL) {
                target = args[i];
            } else {
                directory = args[i];
            }
        }

        if (target == NULL) {
            printf("Missing target argument for seek.\n");
        } else {
            seek_command(target, directory, show_dirs, show_files, execute_flag);
        }
    } else {
        // Handle redirection or pipes if present

        // Fork and execute other commands in a child process
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            // If the process is being run in the background, redirect I/O
            if (run_in_background) {
                int devnull = open("/dev/null", O_RDWR);
                dup2(devnull, STDIN_FILENO);  // Redirect stdin to /dev/null
                dup2(devnull, STDOUT_FILENO); // Redirect stdout to /dev/null
                dup2(devnull, STDERR_FILENO); // Redirect stderr to /dev/null
                close(devnull);
            }

            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent process
            // Add the process to activities

            if (run_in_background) {
                printf("[%d] %d\n", pid, pid);  // Print background process PID
            } else {
                foreground_pid = pid;  // Set the foreground process PID

                struct timeval start_time, end_time;
                gettimeofday(&start_time, NULL);

                int status;
                waitpid(pid, &status, WUNTRACED);  // Wait for the process to finish or stop

                gettimeofday(&end_time, NULL);

                double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                                      (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

                foreground_pid = -1;  // Reset after process exits or stops

                if (elapsed_time >= 2) {
                    strncpy(last_command_name, args[0], sizeof(last_command_name) - 1);
                    last_command_name[sizeof(last_command_name) - 1] = '\0';
                    last_command_time = (int)elapsed_time;
                } else {
                    last_command_name[0] = '\0';
                    last_command_time = 0;
                }

            }
        } else {
            // Fork failed
            perror("fork");
        }
    }

    // Add non-log commands to the log
    if (strstr(original_command, "log") == NULL) {
        add_to_log(original_command);
    }
}

void parse_and_execute(char* input, const char* home_directory) {
    char* command_part;
    char* save_ptr = NULL;
    
    // First, split the input by ';' to handle sequential commands
    command_part = strtok_r(input, ";", &save_ptr);

    while (command_part != NULL) {
        // Trim leading spaces/tabs
        while (*command_part == ' ' || *command_part == '\t') {
            command_part++;
        }

        // Check if the command should run in the background
        int run_in_background = 0;
        int len = strlen(command_part);
        if (len > 0 && command_part[len - 1] == '&') {
            run_in_background = 1;
            command_part[len - 1] = '\0';  // Remove the '&' from the command
            // Trim any extra spaces after removing the '&'
            while (len > 1 && (command_part[len - 2] == ' ' || command_part[len - 2] == '\t')) {
                command_part[len - 2] = '\0';
                len--;
            }
        }

        // Check if it's an alias first
        
            // Handle the command part
        handle_command(command_part, home_directory, run_in_background);

        // Get the next command part
        command_part = strtok_r(NULL, ";", &save_ptr);
    }

    // Check for finished background processes
    int status;
    pid_t finished_pid;
    while ((finished_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("Background process %d exited normally.\n", finished_pid);
        } else if (WIFSTOPPED(status)) {
            printf("Background process %d was stopped.\n", finished_pid);
        } else {
            printf("Background process %d exited abnormally.\n", finished_pid);
        }
    }
}
