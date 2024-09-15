#ifndef SHELL_H
#define SHELL_H

#include <unistd.h>
#include <limits.h>  // For PATH_MAX

#define MAX_PATH_LEN PATH_MAX
#define MAX_PROMPT_LEN 256

// Function to get the username
char* get_username();

// Function to get the system name
char* get_system_name();

// Function to get the current directory
void get_current_directory(char* buffer, size_t size);

// Function to display the shell prompt
void display_prompt(const char* home_directory, const char* last_command, int last_command_time);

#endif // SHELL_H
