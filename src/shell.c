#include "shell.h"
#include <errno.h>

// Global history variables
static char* history[HISTORY_SIZE];
static int history_count = 0;
static int history_index = 0;

// New read_cmd function using readline
char* read_cmd(char* prompt) {
    char* cmdline = readline(prompt);
    
    if (cmdline == NULL) {
        return NULL; // Ctrl+D was pressed
    }
    
    // Add non-empty commands to readline's history
    if (strlen(cmdline) > 0) {
        add_history(cmdline);
    }
    
    return cmdline;
}

// Enhanced tokenize function that handles redirection operators
char** tokenize(char* cmdline) {
    // Edge case: empty command line
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n') {
        return NULL;
    }

    char** arglist = (char**)malloc(sizeof(char*) * (MAXARGS + 1));
    for (int i = 0; i < MAXARGS + 1; i++) {
        arglist[i] = NULL;
    }

    char* cp = cmdline;
    char* start;
    int len;
    int argnum = 0;
    int in_quotes = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++; // Skip leading whitespace
        
        if (*cp == '\0') break; // Line was only whitespace

        start = cp;
        len = 0;
        
        // Handle quoted arguments
        if (*cp == '"' || *cp == '\'') {
            in_quotes = *cp;
            start = ++cp; // Skip opening quote
            while (*cp != '\0' && *cp != in_quotes) {
                len++;
                cp++;
            }
            if (*cp == in_quotes) cp++; // Skip closing quote
        } else {
            // Regular token
            while (*cp != '\0' && *cp != ' ' && *cp != '\t' && 
                   *cp != '|' && *cp != '<' && *cp != '>' && *cp != ';' && *cp != '&') {
                len++;
                cp++;
            }
        }
        
        if (len > 0) {
            arglist[argnum] = (char*)malloc(len + 1);
            strncpy(arglist[argnum], start, len);
            arglist[argnum][len] = '\0';
            argnum++;
        }
    }

    if (argnum == 0) { // No arguments were parsed
        free(arglist);
        return NULL;
    }

    arglist[argnum] = NULL;
    return arglist;
}

// Check if token is a redirection operator
int is_redirection_operator(char* token) {
    return token != NULL && (strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || strcmp(token, ">>") == 0);
}

// Check if token is a pipe operator
int is_pipe_operator(char* token) {
    return token != NULL && strcmp(token, "|") == 0;
}

// Check if token is a chain operator
int is_chain_operator(char* token) {
    return token != NULL && strcmp(token, ";") == 0;
}

// Execute command chain separated by semicolons
int execute_command_chain(char* cmdline) {
    char* saveptr;
    char* token;
    char* cmdline_copy = strdup(cmdline);
    int status = 0;
    
    token = strtok_r(cmdline_copy, ";", &saveptr);
    while (token != NULL) {
        // Trim whitespace
        while (*token == ' ' || *token == '\t') token++;
        char* end = token + strlen(token) - 1;
        while (end > token && (*end == ' ' || *end == '\t' || *end == ';')) end--;
        *(end + 1) = '\0';
        
        if (strlen(token) > 0) {
            pipeline_t* pipeline = parse_command_line(token);
            if (pipeline != NULL && pipeline->num_commands > 0) {
                status = execute_pipeline(pipeline);
                free_pipeline(pipeline);
            }
        }
        
        token = strtok_r(NULL, ";", &saveptr);
    }
    
    free(cmdline_copy);
    return status;
}

// Parse command line with pipes and redirection
pipeline_t* parse_command_line(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0') return NULL;
    
    pipeline_t* pipeline = (pipeline_t*)malloc(sizeof(pipeline_t));
    pipeline->num_commands = 0;
    
    // Tokenize the entire command line
    char** tokens = tokenize(cmdline);
    if (tokens == NULL) {
        free(pipeline);
        return NULL;
    }
    
    command_t* current_cmd = &pipeline->commands[0];
    int arg_index = 0;
    current_cmd->input_file = NULL;
    current_cmd->output_file = NULL;
    current_cmd->append_output = 0;
    current_cmd->background = 0;
    
    for (int i = 0; tokens[i] != NULL && pipeline->num_commands < MAX_COMMANDS; i++) {
        if (is_pipe_operator(tokens[i])) {
            // End current command and start new one
            current_cmd->args[arg_index] = NULL;
            pipeline->num_commands++;
            current_cmd = &pipeline->commands[pipeline->num_commands];
            arg_index = 0;
            current_cmd->input_file = NULL;
            current_cmd->output_file = NULL;
            current_cmd->append_output = 0;
            current_cmd->background = 0;
        } else if (strcmp(tokens[i], "<") == 0) {
            // Input redirection
            if (tokens[i + 1] != NULL) {
                current_cmd->input_file = strdup(tokens[++i]);
            }
        } else if (strcmp(tokens[i], ">") == 0) {
            // Output redirection (truncate)
            if (tokens[i + 1] != NULL) {
                current_cmd->output_file = strdup(tokens[++i]);
                current_cmd->append_output = 0;
            }
        } else if (strcmp(tokens[i], ">>") == 0) {
            // Output redirection (append)
            if (tokens[i + 1] != NULL) {
                current_cmd->output_file = strdup(tokens[++i]);
                current_cmd->append_output = 1;
            }
        } else if (strcmp(tokens[i], "&") == 0 && tokens[i + 1] == NULL) {
            // Background execution (only if & is at the end)
            current_cmd->background = 1;
        } else {
            // Regular argument
            current_cmd->args[arg_index++] = strdup(tokens[i]);
        }
    }
    
    // Finish the last command
    if (arg_index > 0) {
        current_cmd->args[arg_index] = NULL;
        pipeline->num_commands++;
    }
    
    // Free tokens
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
    
    return pipeline;
}

// Free pipeline memory
void free_pipeline(pipeline_t* pipeline) {
    if (pipeline == NULL) return;
    
    for (int i = 0; i < pipeline->num_commands; i++) {
        command_t* cmd = &pipeline->commands[i];
        
        for (int j = 0; cmd->args[j] != NULL; j++) {
            free(cmd->args[j]);
        }
        
        if (cmd->input_file != NULL) free(cmd->input_file);
        if (cmd->output_file != NULL) free(cmd->output_file);
    }
    
    free(pipeline);
}

// Execute a pipeline of commands
int execute_pipeline(pipeline_t* pipeline) {
    if (pipeline == NULL || pipeline->num_commands == 0) return -1;
    
    if (pipeline->num_commands == 1) {
        // Single command (with or without redirection)
        return execute_single_command(&pipeline->commands[0]);
    } else {
        // Multiple commands with pipes
        int pipefds[MAX_COMMANDS - 1][2];
        pid_t pids[MAX_COMMANDS];
        int status;
        
        // Create pipes
        for (int i = 0; i < pipeline->num_commands - 1; i++) {
            if (pipe(pipefds[i]) == -1) {
                perror("pipe");
                return -1;
            }
        }
        
        // Fork child processes
        for (int i = 0; i < pipeline->num_commands; i++) {
            pids[i] = fork();
            
            if (pids[i] == -1) {
                perror("fork");
                return -1;
            }
            
            if (pids[i] == 0) { // Child process
                // Set up pipes
                if (i > 0) {
                    // Not first command: connect stdin to previous pipe
                    dup2(pipefds[i-1][0], STDIN_FILENO);
                }
                
                if (i < pipeline->num_commands - 1) {
                    // Not last command: connect stdout to next pipe
                    dup2(pipefds[i][1], STDOUT_FILENO);
                }
                
                // Close all pipe file descriptors
                for (int j = 0; j < pipeline->num_commands - 1; j++) {
                    close(pipefds[j][0]);
                    close(pipefds[j][1]);
                }
                
                // Execute the command
                execute_single_command(&pipeline->commands[i]);
                exit(1); // Should not reach here if exec succeeds
            }
        }
        
        // Parent process: close all pipe file descriptors
        for (int i = 0; i < pipeline->num_commands - 1; i++) {
            close(pipefds[i][0]);
            close(pipefds[i][1]);
        }
        
        // Wait for all children (unless background)
        int background = 0;
        for (int i = 0; i < pipeline->num_commands; i++) {
            if (pipeline->commands[i].background) {
                background = 1;
                // Add to job list
                char job_cmd[1024] = {0};
                for (int j = 0; pipeline->commands[i].args[j] != NULL; j++) {
                    strcat(job_cmd, pipeline->commands[i].args[j]);
                    strcat(job_cmd, " ");
                }
                add_job(pids[i], job_cmd);
            }
        }
        
        if (!background) {
            for (int i = 0; i < pipeline->num_commands; i++) {
                waitpid(pids[i], &status, 0);
            }
        }
        
        return 0;
    }
}

// Execute a single command with redirection
int execute_single_command(command_t* cmd) {
    if (cmd == NULL || cmd->args[0] == NULL) return -1;
    
    // Handle empty command
    if (cmd->args[0][0] == '\0') return 0;
    
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    
    if (pid == 0) { // Child process
        // Set up redirection
        if (setup_redirection(cmd) != 0) {
            exit(1);
        }
        
        // Execute the command
        execvp(cmd->args[0], cmd->args);
        perror("execvp");
        exit(1);
    } else { // Parent process
        if (!cmd->background) {
            int status;
            waitpid(pid, &status, 0);
            return status;
        } else {
            // Add to job list for background process
            char job_cmd[1024] = {0};
            for (int i = 0; cmd->args[i] != NULL; i++) {
                strcat(job_cmd, cmd->args[i]);
                strcat(job_cmd, " ");
            }
            add_job(pid, job_cmd);
            return 0;
        }
    }
}

// Set up input/output redirection for a command
int setup_redirection(command_t* cmd) {
    // Input redirection
    if (cmd->input_file != NULL) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd == -1) {
            perror("open input file");
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("dup2 stdin");
            close(fd);
            return -1;
        }
        close(fd);
    }
    
    // Output redirection
    if (cmd->output_file != NULL) {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->append_output) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }
        
        int fd = open(cmd->output_file, flags, 0644);
        if (fd == -1) {
            perror("open output file");
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 stdout");
            close(fd);
            return -1;
        }
        close(fd);
    }
    
    return 0;
}

// Old execute function for backward compatibility
int execute(char* arglist[]) {
    if (arglist == NULL || arglist[0] == NULL) return -1;
    
    command_t cmd;
    int i;
    for (i = 0; arglist[i] != NULL && i < MAXARGS - 1; i++) {
        cmd.args[i] = arglist[i];
    }
    cmd.args[i] = NULL;
    cmd.input_file = NULL;
    cmd.output_file = NULL;
    cmd.append_output = 0;
    cmd.background = 0;
    
    return execute_single_command(&cmd);
}

// Rest of the functions remain the same (history, built-ins, etc.)
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
    printf("  jobs              - Show background jobs\n");
    printf("  history           - Show command history\n");
    printf("  !<number>         - Execute command from history\n");
    printf("\nEnhanced Features:\n");
    printf("  Tab Completion    - Press Tab to complete commands and filenames\n");
    printf("  History Navigation - Use Up/Down arrows to browse command history\n");
    printf("  Line Editing      - Full line editing capabilities\n");
    printf("  I/O Redirection   - < (input), > (output), >> (append)\n");
    printf("  Pipes             - | (connect commands)\n");
    printf("  Command Chaining  - ; (sequential execution)\n");
    printf("  Background Jobs   - & (run in background)\n");
    printf("\nExamples:\n");
    printf("  ls -l; pwd; whoami           # Sequential execution\n");
    printf("  sleep 10 &                   # Background job\n");
    printf("  ls -l > file_list.txt        # Output redirection\n");
    printf("  cat file.txt | grep pattern  # Pipe\n");
    printf("\nExternal commands are also supported (ls, pwd, grep, etc.)\n");
}

void execute_jobs() {
    print_jobs();
}

void execute_history() {
    print_history();
}
