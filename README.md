# BSDSF23A039-OS-A03 - Custom Shell Implementation

A custom UNIX shell implementation with advanced features including:
- Built-in commands
- Command history
- Tab completion
- I/O redirection and pipes
- Command chaining and background execution
- if-then-else-fi control structures
- Shell variables

## Building the Shell
```bash
make
./bin/myshell

Dependencies

    GNU Readline library: sudo apt-get install libreadline-dev

Features Implemented

    Base shell functionality

    Built-in commands (exit, cd, help, jobs, history, set)

    Command history (history command and !n execution)

    Tab completion with Readline (command and filename completion)

    I/O redirection and pipes (<, >, >>, |)

    Command chaining and background execution (; and &)

    if-then-else-fi control structure

    Shell variables

Current Built-in Commands

    cd <directory> - Change current working directory

    exit - Exit the shell

    help - Show help message with available commands

    jobs - Show active background jobs

    history - Show command history (last 20 commands)

    set - Show all shell variables

    !<number> - Execute command from history by number

Shell Variables

    Variable Assignment: VARNAME=value

        Example: NAME="John Doe", COUNT=5

    Variable Expansion: $VARNAME

        Example: echo $NAME, ls $HOME

    Environment Variables: Automatic access to system environment variables

    Quoted Values: Support for quoted strings: MSG="Hello World"

Enhanced User Interface

    Tab Completion: Press Tab to complete commands and filenames

    History Navigation: Use Up/Down arrow keys to browse command history

    Line Editing: Full line editing capabilities (move cursor, delete, etc.)

I/O Redirection and Pipes

    Input Redirection: < - Read input from file

    Output Redirection: > - Write output to file (overwrite)

    Append Redirection: >> - Append output to file

    Pipes: | - Connect commands

Command Chaining and Background Execution

    Command Chaining: ; - Execute commands sequentially

    Background Jobs: & - Run command in background

    Job Control: jobs - List active background jobs

Control Structures

    if-then-else-fi: Conditional command execution

        Example:
        bash

if grep "user" /etc/passwd
then
    echo "User found"
else
    echo "User not found"
fi

Examples
bash

# Variables and commands
NAME="Alice"
echo "Hello, $NAME"
COUNT=3; echo "Count: $COUNT"

# Complex example with all features
if [ -f "$HOME/.bashrc" ]
then
    cat $HOME/.bashrc | grep "alias" > aliases.txt
    echo "Aliases saved" &
else
    echo "No .bashrc found"
fi

Student: Yahya Mobeen
Roll No. : BSDSF23A039
Course: Operating Systems
