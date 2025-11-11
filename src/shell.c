#include "shell.h"
#include <errno.h>

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
    printf("\nExternal commands are also supported (ls, pwd, grep, etc.)\n");
}

void execute_jobs() {
    printf("Job control not yet implemented.\n");
}
