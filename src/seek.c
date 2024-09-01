#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "seek.h"

// Function prototypes for internal use
void seek_in_directory(const char *target, const char *path, int show_dirs, int show_files, int execute_flag);
void print_relative_path(const char *base_path, const char *file_path);

static int total_match_count = 0;  // Track total matches across all directories

void seek_command(const char *target, const char *directory, int show_dirs, int show_files, int execute_flag) {
    total_match_count = 0;  // Reset the total match count for each seek command

    // If both -d and -f flags are provided, print "Invalid flags!"
    if (show_dirs && show_files) {
        printf("Invalid flags!\n");
        return;
    }

    // If no directory is provided, use the current directory
    if (directory == NULL) {
        directory = ".";
    }

    // Search for the target in the given directory
    seek_in_directory(target, directory, show_dirs, show_files, execute_flag);

    // After the search is complete, check if any matches were found
    if (total_match_count == 0) {
        printf("No match found!\n");
    }
}


void seek_in_directory(const char *target, const char *path, int show_dirs, int show_files, int execute_flag) {
    struct dirent *entry;
    DIR *dp;
    char full_path[PATH_MAX];
    int local_match_count = 0;  // Tracks matches for this directory
    char single_match_path[PATH_MAX];  // Store the path of a single match
    int is_single_dir = 0, is_single_file = 0;

    dp = opendir(path);
    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Check if the entry starts with the target name
        if (strncmp(entry->d_name, target, strlen(target)) == 0) {
            struct stat entry_stat;
            if (stat(full_path, &entry_stat) != 0) {
                perror("stat");
                continue;
            }

            // Handle directories
            if (S_ISDIR(entry_stat.st_mode) && (show_dirs || (!show_dirs && !show_files))) {
                local_match_count++;
                total_match_count++;
                strcpy(single_match_path, full_path);
                is_single_dir = 1;
                is_single_file = 0;
                print_relative_path(path, entry->d_name);
            }

            // Handle files
            if (S_ISREG(entry_stat.st_mode) && (show_files || (!show_dirs && !show_files))) {
                local_match_count++;
                total_match_count++;
                strcpy(single_match_path, full_path);
                is_single_file = 1;
                is_single_dir = 0;
                print_relative_path(path, entry->d_name);
            }
        }

        // Recurse into directories
        if (entry->d_type == DT_DIR) {
            seek_in_directory(target, full_path, show_dirs, show_files, execute_flag);
        }
    }

    closedir(dp);

    // Handle the -e flag if only one match was found
    if (execute_flag && total_match_count == 1) {
        struct stat match_stat;
        if (stat(single_match_path, &match_stat) == 0) {
            // Handle the case where a single directory is found
            if (is_single_dir && S_ISDIR(match_stat.st_mode)) {
                if (chdir(single_match_path) == 0) {
                    printf("Changed directory to: %s\n", single_match_path);
                } else {
                    perror("chdir");
                }
            }
            // Handle the case where a single file is found
            else if (is_single_file && S_ISREG(match_stat.st_mode)) {
                FILE *file = fopen(single_match_path, "r");
                if (file) {
                    char line[256];
                    while (fgets(line, sizeof(line), file)) {
                        printf("%s", line);
                    }
                    fclose(file);
                } else {
                    perror("fopen");
                }
            }
        }
    }
}


void print_relative_path(const char *base_path, const char *file_path) {
    char relative_path[PATH_MAX];

    // Check if the base_path is current directory "."
    if (strcmp(base_path, ".") == 0) {
        snprintf(relative_path, sizeof(relative_path), "./%s", file_path);  // Just prefix with "./"
    } else {
        snprintf(relative_path, sizeof(relative_path), "%s/%s", base_path, file_path);  // Full relative path
    }

    struct stat file_stat;
    if (stat(relative_path, &file_stat) == 0) {
        if (S_ISDIR(file_stat.st_mode)) {
            printf("\033[1;34m%s\033[0m\n", relative_path);  // Blue for directories
        } else {
            printf("\033[1;32m%s\033[0m\n", relative_path);  // Green for files
        }
    } else {
        printf("%s\n", relative_path);  // Default color
    }
}
