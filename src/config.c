#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "command_handler.h"

// Define structures for alias and function handling
typedef struct {
    char alias[50];
    char command[256];
} Alias;

typedef struct {
    char func_name[50];
    char func_body[256];
} ShellFunction;

// Arrays to store aliases and functions
Alias alias_list[100];
int alias_count = 0;

ShellFunction function_list[100];
int function_count = 0;

void load_myshrc(const char *home_directory) {
    char config_file_path[256];
    snprintf(config_file_path, sizeof(config_file_path), "%s/.myshrc", home_directory);

    FILE *file = fopen(config_file_path, "r");
    if (!file) {
        printf("No .myshrc file found\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  // Strip newline

      //  printf("DEBUG: Processing line: %s\n", line);

        // Handle alias definition
        if (strncmp(line, "alias ", 6) == 0) {
            char *alias_name = strtok(line + 6, "=");
            char *alias_command = strtok(NULL, "=");
            if (alias_name && alias_command) {
                alias_name[strcspn(alias_name, " ")] = '\0'; // Trim spaces
           //     printf("DEBUG: Found alias: %s = %s\n", alias_name, alias_command);
                strcpy(alias_list[alias_count].alias, alias_name);
                strcpy(alias_list[alias_count].command, alias_command);
                alias_count++;
            }
        }
        // Handle function definition
        else if (strncmp(line, "func ", 5) == 0 || strchr(line, '(') != NULL) {
            char *func_name = strtok(line, "(");
            strtok(NULL, "{");  // Skip until the function body

            if (func_name) {
                strcpy(function_list[function_count].func_name, func_name + 5); // Skip "func "
              //  printf("DEBUG: Found function: %s\n", function_list[function_count].func_name);
                function_count++;
            }
        } else if (strstr(line, "}") == NULL) {
            // Function body (ignoring the opening and closing curly braces)
            if (function_count > 0 && strstr(line, "{") == NULL) {
                strcat(function_list[function_count - 1].func_body, line);
                strcat(function_list[function_count - 1].func_body, "\n");
            }
        }
    }

    fclose(file);
}

// Trim leading and trailing spaces from a command string
void trim_whitespace(char *str) {
    char *end;

    // Trim leading spaces
    while (*str == ' ' || *str == '\t') str++;

    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) end--;

    // Null-terminate the trimmed string
    *(end + 1) = '\0';
}

// Replace the input if an alias exists for it
int replace_alias(char *input) {
    for (int i = 0; i < alias_count; i++) {
        char *alias_name = alias_list[i].alias;
     //   printf("DEBUG: Checking alias: '%s' -> '%s'\n", alias_name, alias_list[i].command);

        if (strcmp(input, alias_name) == 0) {
     //       printf("DEBUG: Alias found, replacing '%s' with '%s'\n", input, alias_list[i].command);
            strcpy(input, alias_list[i].command);
            return 1;
        }
    }
    return 0;
}

// Execute a shell function by replacing input with commands
int execute_function(char *command, const char *home_directory) {
    for (int i = 0; i < function_count; i++) {
        if (strncmp(command, function_list[i].func_name, strlen(function_list[i].func_name)) == 0) {
            char *args = command + strlen(function_list[i].func_name) + 1;  // Skip function name and space
            char final_command[256];
            strcpy(final_command, function_list[i].func_body);

      //      printf("DEBUG: Function %s matched, args: %s\n", function_list[i].func_name, args);

            // Trim extra spaces
            while (*args == ' ' || *args == '\t') {
                args++;
            }

            // Replace "$1" in function body with arguments (without adding extra quotes)
            char *placeholder = strstr(final_command, "$1");
            while (placeholder != NULL) {
                char temp_body[256];
                strncpy(temp_body, final_command, placeholder - final_command);
                temp_body[placeholder - final_command] = '\0';
                strcat(temp_body, args);  // Directly append the argument without quotes
                strcat(temp_body, placeholder + 2); // Skip "$1"
                strcpy(final_command, temp_body);

                placeholder = strstr(final_command, "$1");  // Check if there's another $1 to replace
            }

    //        printf("DEBUG: Final function command after substitution: %s\n", final_command);

            // Execute each command within the function body, ignoring braces
            char *func_command = strtok(final_command, "\n");
            while (func_command != NULL) {
                // Ignore any leftover curly braces
                if (strstr(func_command, "{") == NULL && strstr(func_command, "}") == NULL) {
       //             printf("DEBUG: Executing internal function command: %s\n", func_command);
                    // Trim leading and trailing spaces from the command
                    trim_whitespace(func_command);

                    // Now execute the parsed command properly
                    if (strlen(func_command) > 0) { // Ensure non-empty command
                        char command_copy[256];
                        strncpy(command_copy, func_command, sizeof(command_copy));
                        command_copy[sizeof(command_copy) - 1] = '\0';  // Ensure null termination
    //                    printf("DEBUG: Command to be executed: %s\n", command_copy);
                        parse_and_execute(command_copy, home_directory);  // Re-run the command with args
                    }
                }
                func_command = strtok(NULL, "\n");
            }
            return 1;
        }
    }
    return 0;
}
