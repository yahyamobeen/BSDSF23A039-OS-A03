cat > src/parser.c << 'EOF'
#include "shell.h"
#include <string.h>
#include <stdlib.h>

pipeline_t* parse_command_line(char* cmdline) {
    if (cmdline == NULL || strlen(cmdline) == 0) return NULL;
    
    pipeline_t* pipeline = malloc(sizeof(pipeline_t));
    pipeline->num_commands = 0;
    pipeline->background = 0;
    
    // Check for background execution
    char* bg_ptr = strchr(cmdline, '&');
    if (bg_ptr != NULL) {
        pipeline->background = 1;
        *bg_ptr = '\0'; // Remove & from command string
    }
    
    char* saveptr;
    char* token = strtok_r(cmdline, "|", &saveptr);
    
    while (token != NULL && pipeline->num_commands < MAX_COMMANDS) {
        command_t* current_cmd = &pipeline->commands[pipeline->num_commands];
        current_cmd->argc = 0;
        current_cmd->input_file = NULL;
        current_cmd->output_file = NULL;
        current_cmd->append_output = 0;
        
        // Parse individual command with redirection
        char* cmd_saveptr;
        char* cmd_token = strtok_r(token, " \t", &cmd_saveptr);
        int parsing_args = 1;
        
        while (cmd_token != NULL && current_cmd->argc < MAX_ARGS_PER_CMD - 1) {
            if (strcmp(cmd_token, "<") == 0) {
                // Input redirection
                parsing_args = 0;
                cmd_token = strtok_r(NULL, " \t", &cmd_saveptr);
                if (cmd_token != NULL) {
                    current_cmd->input_file = strdup(cmd_token);
                }
            } else if (strcmp(cmd_token, ">") == 0) {
                // Output redirection (truncate)
                parsing_args = 0;
                cmd_token = strtok_r(NULL, " \t", &cmd_saveptr);
                if (cmd_token != NULL) {
                    current_cmd->output_file = strdup(cmd_token);
                    current_cmd->append_output = 0;
                }
            } else if (strcmp(cmd_token, ">>") == 0) {
                // Output redirection (append)
                parsing_args = 0;
                cmd_token = strtok_r(NULL, " \t", &cmd_saveptr);
                if (cmd_token != NULL) {
                    current_cmd->output_file = strdup(cmd_token);
                    current_cmd->append_output = 1;
                }
            } else if (parsing_args) {
                // Regular argument
                current_cmd->args[current_cmd->argc++] = strdup(cmd_token);
            }
            
            cmd_token = strtok_r(NULL, " \t", &cmd_saveptr);
        }
        
        current_cmd->args[current_cmd->argc] = NULL;
        pipeline->num_commands++;
        token = strtok_r(NULL, "|", &saveptr);
    }
    
    return pipeline;
}

void free_pipeline(pipeline_t* pipeline) {
    if (pipeline == NULL) return;
    
    for (int i = 0; i < pipeline->num_commands; i++) {
        command_t* cmd = &pipeline->commands[i];
        
        for (int j = 0; j < cmd->argc; j++) {
            free(cmd->args[j]);
        }
        
        if (cmd->input_file) free(cmd->input_file);
        if (cmd->output_file) free(cmd->output_file);
    }
    
    free(pipeline);
}
EOF
