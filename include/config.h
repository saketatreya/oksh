#ifndef CONFIG_H
#define CONFIG_H

// Function to load aliases and functions from .myshrc file
void load_myshrc(const char *home_directory);

// Function to replace input with alias if found
int replace_alias(char *input);

// Function to execute a function (like mk_hop or hop_seek)
int execute_function(char *command, const char *home_directory);

#endif // CONFIG_H
