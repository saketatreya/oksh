#ifndef LOG_H
#define LOG_H

void add_to_log(char* command);
void print_log();
void execute_from_log(int index, const char* home_directory);
void purge_log();

#endif 
