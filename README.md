# OK Shell 

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

