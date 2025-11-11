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

    !<number> - Execute command from history by number

Enhanced User Interface

    Tab Completion: Press Tab to complete commands and filenames

    History Navigation: Use Up/Down arrow keys to browse command history

    Line Editing: Full line editing capabilities (move cursor, delete, etc.)

I/O Redirection and Pipes

    Input Redirection: < - Read input from file

        Example: sort < input.txt

    Output Redirection: > - Write output to file (overwrite)

        Example: ls -l > file_list.txt

    Append Redirection: >> - Append output to file

        Example: echo "new line" >> file.txt

    Pipes: | - Connect commands

        Example: cat file.txt | grep "pattern" | sort

Command Chaining and Background Execution

    Command Chaining: ; - Execute commands sequentially

        Example: ls -l; pwd; whoami

    Background Jobs: & - Run command in background

        Example: sleep 10 &

    Job Control: jobs - List active background jobs

    Zombie Prevention: Automatic cleanup of completed processes

Student: BSDSF23A039
Course: Operating Systems
