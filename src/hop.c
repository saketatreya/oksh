#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "hop.h"

void hop_command(char* args[], int argc, const char* home_directory) {
    char prev_directory[PATH_MAX];
    static char old_directory[PATH_MAX] = "";

    // Get the current working directory before attempting the hop
    if (getcwd(prev_directory, sizeof(prev_directory)) == NULL) {
        perror("getcwd (before hop)");
        return;
    }

    for (int i = 0; i < argc; i++) {
        char* target_dir = args[i];
        char resolved_path[PATH_MAX];

        // Handle special cases for target directories
        if (strcmp(target_dir, "~") == 0) {
            target_dir = (char*)home_directory;
        } else if (strcmp(target_dir, "-") == 0) {
            if (strlen(old_directory) == 0) {
                printf("OLDPWD not set\n");
                continue;
            }
            target_dir = old_directory;
        } else if (strncmp(target_dir, "~/", 2) == 0) {
            snprintf(resolved_path, sizeof(resolved_path), "%s%s", home_directory, target_dir + 1);
            target_dir = resolved_path;
        }

        // Attempt to change directory
        if (chdir(target_dir) == 0) {
            // Update old_directory only if the change was successful
            strcpy(old_directory, prev_directory);

            // Get new working directory and print it
            if (getcwd(prev_directory, sizeof(prev_directory)) != NULL) {
                printf("%s\n", prev_directory);
            } else {
                perror("getcwd (after successful chdir)");
            }
        } else {
            perror("chdir (directory change failed)");
        }
    }

    // If no arguments are given, hop to the home directory
    if (argc == 0) {
        if (chdir(home_directory) != 0) {
            perror("chdir (to home directory)");
        } else {
            if (getcwd(prev_directory, sizeof(prev_directory)) != NULL) {
                printf("%s\n", prev_directory);
            } else {
                perror("getcwd (after changing to home)");
            }
        }
    }
}
