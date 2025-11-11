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

    Built-in commands (exit, cd, help, jobs)

    Command history (history command and !n execution)

    Tab completion with Readline (command and filename completion)

    I/O redirection and pipes

    Command chaining and background execution

    if-then-else-fi control structure

    Shell variables

Current Built-in Commands

    cd <directory> - Change current working directory

    exit - Exit the shell

    help - Show help message with available commands

    jobs - Placeholder for job control (not yet implemented)

    history - Show command history (last 20 commands)

    !<number> - Execute command from history by number

Enhanced User Interface

    Tab Completion: Press Tab to complete commands and filenames

    History Navigation: Use Up/Down arrow keys to browse command history

    Line Editing: Full line editing capabilities (move cursor, delete, etc.)

    Command History: Persistent command history across sessions

History Features

    Stores last 20 commands in circular buffer

    history command displays numbered command list

    !n re-executes the nth command from history

    Prevents duplicate consecutive commands in history

Student: BSDSF23A039
Course: Operating Systems
