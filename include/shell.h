#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LEN 1024
#define MAXARGS 64
#define ARGLEN 64
#define PROMPT "myshell> "
#define HISTORY_SIZE 20

// Function declarations
char* read_cmd(char* prompt);
char** tokenize(char* cmdline);
int execute(char* arglist[]);

// Built-in command functions
int handle_builtin(char** arglist);
void execute_cd(char** args);
void execute_help();
void execute_jobs();
void execute_history();

// History functions
void add_to_history(const char* command);
void print_history();
char* get_history_command(int n);
void handle_history_execution(char** cmdline);

#endif
