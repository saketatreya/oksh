#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"

// ANSI color codes
#define RESET_COLOR "\033[0m"
#define USERNAME_COLOR "\033[1;32m"  // Bright green
#define SYSTEM_NAME_COLOR "\033[1;34m"  // Bright blue
#define DIRECTORY_COLOR "\033[1;33m"  // Bright yellow
#define COMMAND_COLOR "\033[1;35m"  // Bright magenta
#define TIME_COLOR "\033[1;31m"  // Bright red

char* get_username() {
    return getenv("USER");
}

char* get_system_name() {
    static char system_name[MAX_PATH_LEN];
    gethostname(system_name, sizeof(system_name));
    return system_name;
}

void get_current_directory(char* buffer, size_t size) {
    getcwd(buffer, size);  // Always get the current working directory
}

void display_prompt(const char* home_directory, const char* last_command_name, int last_command_time) {
    char current_directory[MAX_PATH_LEN];
    
    // Get the current directory
    get_current_directory(current_directory, sizeof(current_directory));  

    // Check if the current directory is inside the home directory
    const char* relative_path = current_directory;
    if (strncmp(current_directory, home_directory, strlen(home_directory)) == 0) {
        relative_path = current_directory + strlen(home_directory);
        if (*relative_path == '\0') {
            relative_path = "~";
        } else {
            snprintf(current_directory, sizeof(current_directory), "~%s", relative_path);
            relative_path = current_directory;
        }
    }

    // Get the username and system name
    char* username = get_username();
    char* system_name = get_system_name();

    // Display the prompt with colors
    if (last_command_time > 0) {
        printf("%s<%s%s@%s%s:%s%s %s%s : %s%ds%s> ", 
               RESET_COLOR, USERNAME_COLOR, username, SYSTEM_NAME_COLOR, system_name, 
               DIRECTORY_COLOR, relative_path, COMMAND_COLOR, last_command_name, 
               TIME_COLOR, last_command_time, RESET_COLOR);
    } else {
        printf("%s<%s%s@%s%s:%s%s> %s", 
               RESET_COLOR, USERNAME_COLOR, username, SYSTEM_NAME_COLOR, system_name, 
               DIRECTORY_COLOR, relative_path, RESET_COLOR);
    }
}
