#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_LEN 1024
#define MAXARGS 64
#define ARGLEN 64
#define PROMPT "myshell> "
#define HISTORY_SIZE 20
#define MAX_COMMANDS 10
#define MAX_JOBS 100
#define MAX_IF_BLOCKS 10

// Structure for background job
typedef struct {
    pid_t pid;
    char* command;
    int job_id;
    int status; // 0=running, 1=completed, 2=stopped
} job_t;

// Structure for command with redirection
typedef struct {
    char* args[MAXARGS];
    char* input_file;
    char* output_file;
    int append_output;
    int background;
} command_t;

// Structure for pipeline
typedef struct {
    command_t commands[MAX_COMMANDS];
    int num_commands;
} pipeline_t;

// Structure for if-then-else block
typedef struct {
    char* condition_command;
    char* then_commands[MAX_COMMANDS];
    char* else_commands[MAX_COMMANDS];
    int then_count;
    int else_count;
    int has_else;
} if_block_t;

// Global job list
extern job_t job_list[MAX_JOBS];
extern int job_count;

// Function declarations
char* read_cmd(char* prompt);
char* read_multiline_cmd(char* prompt);
char** tokenize(char* cmdline);
int execute(char* arglist[]);

// Enhanced parsing functions
pipeline_t* parse_command_line(char* cmdline);
void free_pipeline(pipeline_t* pipeline);
int is_redirection_operator(char* token);
int is_pipe_operator(char* token);
int is_chain_operator(char* token);

// Execution functions
int execute_pipeline(pipeline_t* pipeline);
int execute_single_command(command_t* cmd);
int execute_command_chain(char* cmdline);
int execute_if_block(if_block_t* if_block);
int setup_redirection(command_t* cmd);
int setup_pipes(pipeline_t* pipeline, int pipefds[][2]);

// Control structure functions
int is_control_structure(char* cmdline);
if_block_t* parse_if_structure();
int execute_condition(char* condition);

// Job control functions
void init_jobs();
void add_job(pid_t pid, char* command);
void remove_job(pid_t pid);
void update_jobs();
void print_jobs();
void cleanup_zombies();

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
