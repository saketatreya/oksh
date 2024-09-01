#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <sys/types.h>

// Struct to hold process information
typedef struct {
    char command_name[256];
    pid_t pid;
    int is_running;  // 1 for running, 0 for stopped
} ProcessInfo;

// Expose the global array and count for activities
extern ProcessInfo activities_list[];
extern int activities_count;

// Function to add a new process to the activities list
void add_process_to_activities(const char* command_name, pid_t pid);

// Function to update the process state in activities
void update_process_state(pid_t pid, int is_running);

// Function to print all active processes in lexicographic order
void print_activities();

#endif // ACTIVITIES_H
