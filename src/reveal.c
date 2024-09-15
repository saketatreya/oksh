#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include "reveal.h"

void print_file_info(const char* path, const struct dirent* entry, int show_long) {
    struct stat file_stat;
    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

    if (stat(full_path, &file_stat) == 0) {
        // Determine the color based on file type
        const char* color;
        if (S_ISDIR(file_stat.st_mode)) {
            color = "\033[1;34m";  // Blue for directories
        } else if (file_stat.st_mode & S_IXUSR) {
            color = "\033[1;32m";  // Green for executables
        } else {
            color = "\033[0m";     // White for regular files
        }

        if (show_long) {
            // Print file type
            printf(S_ISDIR(file_stat.st_mode) ? "d" : "-");
            // Print permissions
            printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
            printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
            printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
            printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
            printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
            printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
            printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
            printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
            printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");

            // Print number of hard links
            printf(" %ld", (long)file_stat.st_nlink);

            // Print owner and group
            struct passwd* pwd = getpwuid(file_stat.st_uid);
            struct group* grp = getgrgid(file_stat.st_gid);
            printf(" %s %s", pwd->pw_name, grp->gr_name);

            // Print file size
            printf(" %5lld", (long long)file_stat.st_size);

            // Print time of last modification
            char time_buf[64];
            strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&file_stat.st_mtime));
            printf(" %s", time_buf);
        }

        // Print file name with color coding
        printf(" %s%s\033[0m\n", color, entry->d_name);
    } else {
        perror("stat");
    }
}

void reveal_command(char* path, int show_all, int show_long, const char* home_directory) {
    DIR* dir;
    struct dirent* entry;
    char resolved_path[PATH_MAX];

    // Resolve the path
    if (strcmp(path, "~") == 0) {
        path = (char*)home_directory;
    } else if (strncmp(path, "~/", 2) == 0) {
        snprintf(resolved_path, sizeof(resolved_path), "%s%s", home_directory, path + 1);
        path = resolved_path;
    }

    if ((dir = opendir(path)) == NULL) {
        perror("opendir");
        return;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files if show_all is not set
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        // Print file information with color coding
        print_file_info(path, entry, show_long);
    }

    closedir(dir);
}
