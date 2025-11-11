#include "shell.h"
#include <errno.h>

// Global history variables
static char* history[HISTORY_SIZE];
static int history_count = 0;
static int history_index = 0;

char* read_cmd(char* prompt, FILE* fp) {
    printf("%s", prompt);
    char* cmdline = (char*) malloc(sizeof(char) * MAX_LEN);
    int c, pos = 0;

    while ((c = getc(fp)) != EOF) {
        if (c == '\n') break;
        cmdline[pos++] = c;
    }

    if (c == EOF && pos == 0) {
        free(cmdline);
        return NULL; // Handle Ctrl+D
    }
    
    cmdline[pos] = '\0';
    return cmdline;
}

char** tokenize(char* cmdline) {
    // Edge case: empty command line
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n') {
        return NULL;
    }

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = (char*)malloc(sizeof(char) * ARGLEN);
        memset(arglist[i], 0, ARGLEN);
    }

    char* cp = cmdline;
    char* start;
    int len;
    int argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++; // Skip leading whitespace
        
        if (*cp == '\0') break; // Line was only whitespace

        start = cp;
        len = 1;
        while (*++cp != '\0' && !(*cp == ' ' || *cp == '\t')) {
            len++;
        }
        strncpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }

    if (argnum == 0) { // No arguments were parsed
        for(int i = 0; i < MAXARGS + 1; i++) free(arglist[i]);
        free(arglist);
        return NULL;
    }

    arglist[argnum] = NULL;
    return arglist;
}

// History functions implementation
void add_to_history(const char* command) {
    if (command == NULL || strlen(command) == 0) return;
    
    // Don't add duplicate consecutive commands
    if (history_count > 0 && history[history_index] != NULL && 
        strcmp(history[history_index], command) == 0) {
        return;
    }
    
    history_index = (history_index + 1) % HISTORY_SIZE;
    if (history[history_index] != NULL) {
        free(history[history_index]);
    }
    history[history_index] = strdup(command);
    
    if (history_count < HISTORY_SIZE) {
        history_count++;
    }
}

void print_history() {
    if (history_count == 0) {
        printf("No commands in history.\n");
        return;
    }
    
    int start = (history_index - history_count + 1 + HISTORY_SIZE) % HISTORY_SIZE;
    for (int i = 0; i < history_count; i++) {
        int idx = (start + i) % HISTORY_SIZE;
        printf("%d %s\n", i + 1, history[idx]);
    }
}

char* get_history_command(int n) {
    if (n < 1 || n > history_count) return NULL;
    
    int start = (history_index - history_count + 1 + HISTORY_SIZE) % HISTORY_SIZE;
    int idx = (start + n - 1) % HISTORY_SIZE;
    return strdup(history[idx]);
}

void handle_history_execution(char** cmdline) {
    if (*cmdline == NULL || (*cmdline)[0] != '!') return;
    
    if (strlen(*cmdline) == 1) {
        printf("Usage: !<number>\n");
        return;
    }
    
    char* endptr;
    int n = strtol((*cmdline) + 1, &endptr, 10);
    
    if (endptr == (*cmdline) + 1 || *endptr != '\0') {
        printf("Invalid history number: %s\n", (*cmdline) + 1);
        return;
    }
    
    if (n < 1 || n > history_count) {
        printf("No such command in history: %d (history size: %d)\n", n, history_count);
        return;
    }
    
    char* historic_cmd = get_history_command(n);
    if (historic_cmd) {
        free(*cmdline);
        *cmdline = historic_cmd;
        printf("Executing: %s\n", historic_cmd);
    }
}

// Built-in command handler
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
    }
    return 0;
}

void execute_cd(char** args) {
    if (args[1] == NULL) {
        // No argument provided, change to home directory
        char* home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME environment variable not set\n");
            return;
        }
        if (chdir(home) != 0) {
            perror("cd");
        }
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
}

void execute_help() {
    printf("Built-in commands:\n");
    printf("  cd <directory>    - Change current directory\n");
    printf("  exit              - Exit the shell\n");
    printf("  help              - Show this help message\n");
    printf("  jobs              - Show background jobs (not yet implemented)\n");
    printf("  history           - Show command history\n");
    printf("  !<number>         - Execute command from history\n");
    printf("\nExternal commands are also supported (ls, pwd, grep, etc.)\n");
}

void execute_jobs() {
    printf("Job control not yet implemented.\n");
}

void execute_history() {
    print_history();
}
