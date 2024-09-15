# Custom Shell Project

## Project Overview

This project is a custom shell implemented in C, providing several built-in commands and utilities. The shell supports typical shell operations like command execution, process management, input/output redirection, and built-in commands (`hop`, `reveal`, `proclore`, `seek`, etc.). It also provides a history-like command log and an interactive prompt with customizable color schemes.

## Features

1. **Custom Commands**:
    - `hop`: Change the current directory.
    - `reveal`: List files and directories with optional detailed information.
    - `proclore`: Display detailed information about a process.
    - `seek`: Search for files or directories starting with a given target.
    - `log`: Tracks commands entered in the shell.
    - `ping`: Send signals to processes.
    - `neonate`: Prints the most recently created process's PID at regular intervals.
    - `fg`, `bg`: Bring a process to the foreground or send it to the background.
    - `iMan`: Fetches the manual page for a command from man.he.net.

2. **Command Execution**:
    - Executes system commands, supports background processes using `&`, and sequential execution using `;`.

3. **Process Management**:
    - Keeps track of running and stopped processes using an activities list.
    - Allows manipulation of processes (e.g., bringing a stopped process to the foreground using `fg`).

4. **Signal Handling:**
    - Responds to `Ctrl-C`, `Ctrl-Z`, and `Ctrl-D` to interrupt, stop, or exit the shell.
    - Allows sending signals to processes using the ping command.

5. **I/O Redirection:**
    - Supports input (`<`), output (`>`), and append (`>>`) redirection.

6.  **Command Log**:
    - Tracks the last 15 unique commands across sessions and provides options to purge or execute commands from the log.

7. **Interactive Prompt**:
    - Displays an interactive prompt with username, system name, current directory, last executed command, and its time, in a colorful format.

## Getting Started


### Prerequisities

- A Unix-like operating system
- GCC (GNU Compiler Collection) for compilation.
- Basic knowledge of C programming and Unix shell commands. 

### Compilation

To compile the shell, run the following command:
```
make
```

This will use the `Makefile` to compile all the source files and produce an executable named `shell`.

## Usage
Run the shell using: 
```
./shell
```

### Commands
Here are the details of each command and its usage:

#### `hop`:

- Usage: `hop <directory>` – Changes to the specified directory.
- Supports special symbols like ~, -, ...

#### `reveal`:

- Usage: `reveal [-a] [-l] <directory>` – Lists files and directories with optional flags:
    - `-a`: Show all files (including hidden files).
    - `-l`: Show detailed information.

#### `proclore`:

- Usage: `proclore [pid]` – Displays information about the specified process.

#### `seek`:

- Usage: `seek [-d | -f] [-e] <target> [directory]` – Searches for files or directories starting with the given target.
    - `-d`: Search only for directories.
    - `-f`: Search only for files.
    - `-e`: Execute the found file or change to the found directory if there is exactly one match.

#### `log`:

- Usage: `log` – Displays the history of commands.

- Usage: `log purge` – Clears the command history.
- Usage: `log execute <index>` – Executes a command from the log by its index.

#### `ping`:

- Usage: `ping <pid> <signal_number>` – Sends a signal to a process with the specified PID.

#### `neonate`:

- Usage: `neonate -n <interval>` – Prints the PID of the most recent process every specified interval (in seconds).

#### `fg`:

- Usage: `fg <pid>` – Brings a stopped process to the foreground.
bg:

- Usage: `bg <pid>` – Resumes a stopped process in the background.

#### `iMan`:

- Usage: `iMan <command>` – Fetches the manual page of the specified command from man.he.net.


### Signal Handling

- `Ctrl-C`: Sends SIGINT to the current foreground process.
- `Ctrl-Z`: Sends SIGTSTP to the current foreground process.
- `Ctrl-D`: Exits the shell after killing all running processes.

### I/O Redirection
Supports input (`<`), output (`>`), and append (`>>`) redirection for commands.
Supports piping (`|`) for command chaining.

### Example Usage

```
<saket@Sakets-MacBook-Air.local:~> reveal -a -l .
<saket@Sakets-MacBook-Air.local:~> hop ..
<saket@Sakets-MacBook-Air.local:~> log
1: reveal -a -l .
2: hop ..
<saket@Sakets-MacBook-Air.local:~> log execute 1
```

## Prompts and Responses for ChatGPT Usage

To provide a more hands-on learning and coding experience, the following prompts and responses were used to develop this shell. These helped solve specific implementation issues without making the project feel entirely created by ChatGPT:

**Prompt:** *"How do I implement a custom command history in a shell, and how can I skip consecutive duplicates?"*

**Response:** "You can maintain a circular buffer to store the last N commands, ensuring duplicate consecutive commands are skipped. To handle duplicates, compare the current command with the last entry before adding it to the history."

**Prompt:** *"What are the steps to handle background and foreground process management in a shell?"*

**Response:** "Use `fork()` to create new processes and `waitpid()` to manage them. For background processes, you need to track their PIDs and use `&` to indicate background execution. To bring a background process to the foreground, use `tcsetpgrp()` to give it terminal control."

**Prompt:** *"Can you guide me on adding colors to my shell prompt in C?"*
**Response:** "Use ANSI escape codes in your printf statements to add colors. For example, `\033[1;32m` sets text to bright green. Remember to reset the colors using `\033[0m` after the prompt is printed."

**Prompt:** *"How can I parse and execute commands with both '&' and ';' operators in my custom shell?"*
**Response:** "First, split the input string by ; to handle sequential commands, then check each command for `&` to run them in the background. Use `fork()` and `execvp()` for command execution and handle background processes without calling waitpid() immediately."

**Prompt:** *"How can I implement input/output redirection and piping in my shell?"*
**Response:** "Use `dup2()` to redirect file descriptors for input (`<`) and output (`>`, `>>`). For piping (`|`), use `pipe()` to create a pipe and `dup2()` to connect the pipe's read and write ends to the appropriate file descriptors before executing commands."

**Prompt:** "What's the best way to handle aliases and functions in a shell configuration file?*

**Response:** "Store aliases and functions in a configuration file (e.g., .myshrc). Read the file at shell startup, use strtok() to parse alias definitions, and store them in a list. Replace the command with its alias before execution."
