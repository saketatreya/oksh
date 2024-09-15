#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "io_redirection.h"
#include "command_handler.h"

// Function to handle I/O redirection for a single command (without pipes)
int handle_io_redirection_single_command(char* command) {
    char *input_file = NULL, *output_file = NULL;
    int append_mode = 0;  // 1 for '>>', 0 for '>'
    char *cmd_copy = strdup(command);
    char *part = strtok(cmd_copy, " ");

    // Parse command for I/O redirection
    while (part != NULL) {
        if (strcmp(part, ">") == 0 || strcmp(part, ">>") == 0) {
            append_mode = strcmp(part, ">>") == 0;
            part = strtok(NULL, " ");
            if (part == NULL) {
                free(cmd_copy);
                return 1;
            }
            output_file = part;
        } else if (strcmp(part, "<") == 0) {
            part = strtok(NULL, " ");
            if (part == NULL) {
                free(cmd_copy);
                return 1;
            }
            input_file = part;
        }
        part = strtok(NULL, " ");
    }

    // Fork to execute the command with I/O redirection
    pid_t pid = fork();
    if (pid == 0) {
        // Child process

        if (input_file) {
            int fd_in = open(input_file, O_RDONLY);
            if (fd_in == -1) {
                printf("No such input file found!\n");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (output_file) {
            int fd_out;
            if (append_mode) {
                fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {
                fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            if (fd_out == -1) {
                perror("open output file");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        // Execute the command
        char *args[10];
        int argc = 0;
        char *token = strtok(command, " ");
        while (token != NULL && argc < 9) {
            if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0 || strcmp(token, "<") == 0) {
                strtok(NULL, " ");  // Skip the next token (filename)
            } else {
                args[argc] = token;
                argc++;
            }
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);  // Wait for the child process to finish
    }

    free(cmd_copy);
    return 0;
}

// Function to handle pipes and I/O redirection combined
int handle_pipes_and_redirection(char* input, const char* home_directory) {
    int pipe_fds[2], status;
    pid_t pid;
    char *commands[10];
    int num_commands = 0;

    // Split input by pipes
    char *cmd = strtok(input, "|");
    while (cmd != NULL && num_commands < 10) {
        commands[num_commands++] = cmd;
        cmd = strtok(NULL, "|");
    }

    // Check for invalid use of pipes
    if (num_commands == 0 || (num_commands == 1 && strstr(input, "|"))) {
        printf("Invalid use of pipe\n");
        return 1;
    }

    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    int append_mode = 0;

    // Handle input redirection in the first command if necessary
    char *first_command = commands[0];
    if (strstr(first_command, "<") != NULL) {
        // Parse the input redirection
        char *input_file = strchr(first_command, '<');
        *input_file = '\0';  // Terminate the command before the '<'
        input_file = strtok(input_file + 1, " ");
        input_fd = open(input_file, O_RDONLY);
        if (input_fd == -1) {
            printf("No such input file found!\n");
            return 1;
        }
    }

    // Handle output redirection in the last command if necessary
    char *last_command = commands[num_commands - 1];
    if (strstr(last_command, ">") != NULL || strstr(last_command, ">>") != NULL) {
        // Parse the output redirection
        char *output_file = strchr(last_command, '>');
        append_mode = (*(output_file + 1) == '>');
        *output_file = '\0';  // Terminate the command before the '>'
        output_file = strtok(output_file + (append_mode ? 2 : 1), " ");
        
        output_fd = open(output_file, O_WRONLY | O_CREAT | (append_mode ? O_APPEND : O_TRUNC), 0644);
        if (output_fd == -1) {
            perror("open output file");
            return 1;
        }
    }

    // Execute the commands in the pipe chain
    for (int i = 0; i < num_commands; i++) {
        pipe(pipe_fds);
        pid = fork();

        if (pid == 0) {
            // Child process

            // Handle input redirection for the first command
            if (i == 0 && input_fd != STDIN_FILENO) {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }

            // Handle output redirection for the last command
            if (i == num_commands - 1 && output_fd != STDOUT_FILENO) {
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
            }

            // For other commands in between
            if (i > 0) {
                dup2(input_fd, STDIN_FILENO);
            }
            if (i < num_commands - 1) {
                dup2(pipe_fds[1], STDOUT_FILENO);
            }

            close(pipe_fds[0]);
            close(pipe_fds[1]);

            // Parse and execute the command
            char *args[10];
            int argc = 0;
            char *token = strtok(commands[i], " ");
            while (token != NULL && argc < 9) {
                args[argc++] = token;
                token = strtok(NULL, " ");
            }
            args[argc] = NULL;

            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            waitpid(pid, &status, 0);
            close(pipe_fds[1]);
            input_fd = pipe_fds[0];  // Pass the read end of the pipe as input for the next command
        }
    }

    return 0;
}
