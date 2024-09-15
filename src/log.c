#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "command_handler.h"

#define LOG_FILE "command_log.txt"
#define MAX_LOG_SIZE 15

// Helper function to load log from file
void load_log(char log_data[MAX_LOG_SIZE][1024], int* log_count) {
    FILE* file = fopen(LOG_FILE, "r");
    if (!file) return;

    *log_count = 0;
    while (fgets(log_data[*log_count], 1024, file)) {
        log_data[*log_count][strcspn(log_data[*log_count], "\n")] = '\0';
        (*log_count)++;
    }
    fclose(file);
}

// Helper function to save log to file
void save_log(char log_data[MAX_LOG_SIZE][1024], int log_count) {
    FILE* file = fopen(LOG_FILE, "w");
    if (!file) return;

    for (int i = 0; i < log_count; i++) {
        fprintf(file, "%s\n", log_data[i]);
    }
    fclose(file);
}

// Function to add a command to the log
void add_to_log(char* command) {
    char log_data[MAX_LOG_SIZE][1024];
    int log_count = 0;
    load_log(log_data, &log_count);

    // If the previous command is the same, skip it
    if (log_count > 0 && strcmp(log_data[log_count - 1], command) == 0) {
        return;
    }

    // If log is full, shift the oldest command out
    if (log_count >= MAX_LOG_SIZE) {
        for (int i = 1; i < MAX_LOG_SIZE; i++) {
            strcpy(log_data[i - 1], log_data[i]);
        }
        log_count--;
    }

    strcpy(log_data[log_count], command);
    log_count++;
    save_log(log_data, log_count);
}

// Function to display the log
void print_log() {
    char log_data[MAX_LOG_SIZE][1024];
    int log_count = 0;
    load_log(log_data, &log_count);

    if (log_count == 0) {
        printf("No commands in log.\n");
        return;
    }

    for (int i = 0; i < log_count; i++) {
        printf("%d: %s\n", i + 1, log_data[i]);
    }
}

// Function to execute a command from the log
void execute_from_log(int index, const char* home_directory) {
    char log_data[MAX_LOG_SIZE][1024];
    int log_count = 0;
    load_log(log_data, &log_count);

    if (index < 1 || index > log_count) {
        printf("No such command in log.\n");
        return;
    }

    // Execute the command at the given index using the shell's environment
    printf("Executing command: %s\n", log_data[index - 1]);
    handle_command(log_data[index - 1], home_directory, 0);  // Execute within the shell environment
}

// Function to purge the log
void purge_log() {
    FILE* file = fopen(LOG_FILE, "w");
    if (file) {
        fclose(file);
    }
}
