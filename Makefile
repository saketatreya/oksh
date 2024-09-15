# Makefile for the shell project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Iinclude -Wall -Wextra -g

# Executable name
TARGET = shell

# Source files
SRCS = src/command_handler.c src/main.c src/shell.c src/hop.c src/reveal.c src/proclore.c src/seek.c src/config.c src/io_redirection.c src/activities.c src/neonate.c src/iman.c src/fgbg.c src/ping.c src/log.c

# Object files (replace .c with .o)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
