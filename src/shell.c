// In the handle_builtin function, add set command:
int handle_builtin(char** arglist) {
    if (arglist[0] == NULL) return 0;
    
    if (strcmp(arglist[0], "exit") == 0) {
        printf("Shell exited.\n");
        exit(0);
    } else if (strcmp(arglist[0], "cd") == 0) {
        execute_cd(arglist);
        return 1;
    } else if (strcmp(arglist[0], "help") == 0) {
        execute_help();
        return 1;
    } else if (strcmp(arglist[0], "jobs") == 0) {
        execute_jobs();
        return 1;
    } else if (strcmp(arglist[0], "history") == 0) {
        execute_history();
        return 1;
    } else if (strcmp(arglist[0], "set") == 0) {
        execute_set();
        return 1;
    }
    return 0;
}

// Add the set command implementation
void execute_set() {
    print_variables();
}

// Update the help command
void execute_help() {
    printf("Built-in commands:\n");
    printf("  cd <directory>    - Change current directory\n");
    printf("  exit              - Exit the shell\n");
    printf("  help              - Show this help message\n");
    printf("  jobs              - Show background jobs\n");
    printf("  history           - Show command history\n");
    printf("  set               - Show all shell variables\n");
    printf("  !<number>         - Execute command from history\n");
    printf("\nVariable Assignment:\n");
    printf("  VARNAME=value     - Set a shell variable\n");
    printf("  $VARNAME          - Use variable in commands\n");
    printf("\nEnhanced Features:\n");
    printf("  Tab Completion    - Press Tab to complete commands and filenames\n");
    printf("  History Navigation - Use Up/Down arrows to browse command history\n");
    printf("  Line Editing      - Full line editing capabilities\n");
    printf("  I/O Redirection   - < (input), > (output), >> (append)\n");
    printf("  Pipes             - | (connect commands)\n");
    printf("  Command Chaining  - ; (sequential execution)\n");
    printf("  Background Jobs   - & (run in background)\n");
    printf("  Control Structures- if-then-else-fi conditional execution\n");
    printf("\nExamples:\n");
    printf("  NAME=\"John Doe\"                 # Set variable\n");
    printf("  echo $NAME                      # Use variable\n");
    printf("  COUNT=5; echo \"Count: $COUNT\"   # Chain with variables\n");
    printf("  if [ \"$NAME\" = \"John Doe\" ]; then echo \"Match\"; fi\n");
    printf("\nExternal commands are also supported (ls, pwd, grep, etc.)\n");
}
