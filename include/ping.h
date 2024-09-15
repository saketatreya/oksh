#ifndef PING_H
#define PING_H

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void ping_command(pid_t pid, int signal_num);

#endif // PING_H
