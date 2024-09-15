#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "activities.h"

// Global array to store process information
#define MAX_PROCESSES 100
ProcessInfo activities_list[MAX_PROCESSES];
int activities_count = 0;

// Function to add a new process to the activities list
void add_process_to_activities(const char* command_name, pid_t pid) {
    if (activities_count < MAX_PROCESSES) {
        strncpy(activities_list[activities_count].command_name, command_name, sizeof(activities_list[activities_count].command_name) - 1);
        activities_list[activities_count].command_name[sizeof(activities_list[activities_count].command_name) - 1] = '\0';
        activities_list[activities_count].pid = pid;
        activities_list[activities_count].is_running = 1;  // Process starts as running
        activities_count++;
    } else {
        printf("ERROR: Max process limit reached!\n");
    }
}

// Function to update the process state in activities
void update_process_state(pid_t pid, int is_running) {
    for (int i = 0; i < activities_count; i++) {
        if (activities_list[i].pid == pid) {
            activities_list[i].is_running = is_running;
            return;
        }
    }
}

// Helper function to check and update the status of processes
void check_and_update_processes() {
    int status;
    pid_t result;

    for (int i = 0; i < activities_count; i++) {
        if (activities_list[i].is_running) {
            result = waitpid(activities_list[i].pid, &status, WNOHANG);
            if (result == 0) {
                // Process is still running
            } else if (result > 0) {
                // Process finished
                activities_list[i].is_running = 0;
            } else {
                perror("waitpid");
            }
        }
    }
}

// Helper function for sorting processes lexicographically by command name
int compare_processes(const void* a, const void* b) {
    ProcessInfo* p1 = (ProcessInfo*)a;
    ProcessInfo* p2 = (ProcessInfo*)b;
    return strcmp(p1->command_name, p2->command_name);
}

// Function to print all active processes in lexicographic order
void print_activities() {
    check_and_update_processes();  // Update process states before printing

    if (activities_count == 0) {
        printf("No active processes.\n");
        return;
    }

    // Sort the processes lexicographically by command name
    qsort(activities_list, activities_count, sizeof(ProcessInfo), compare_processes);

    // Print the processes in the desired format
    for (int i = 0; i < activities_count; i++) {
        printf("[%d] : %s - %s\n", activities_list[i].pid, activities_list[i].command_name,
               activities_list[i].is_running ? "Running" : "Stopped");
    }
}
