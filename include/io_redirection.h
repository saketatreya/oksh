#ifndef IO_REDIRECTION_H
#define IO_REDIRECTION_H

int handle_io_redirection(char* command);
int handle_io_redirection_single_command(char* command);  // Add this declaration
int handle_pipes_and_redirection(char* input, const char* home_directory);  // Add this declaration

#endif // IO_REDIRECTION_H
