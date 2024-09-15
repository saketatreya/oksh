#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

// Function to handle a single command
void handle_command(char* command, const char* home_directory, int run_in_background);

// Function to parse and execute multiple commands
void parse_and_execute(char* input, const char* home_directory);


#endif // COMMAND_HANDLER_H
