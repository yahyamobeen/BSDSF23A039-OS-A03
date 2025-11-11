#include "shell.h"

int main() {
    char* cmdline;
    pipeline_t* pipeline;

    // Initialize job system
    init_jobs();
    
    // Initialize readline for better tab completion
    rl_bind_key('\t', rl_complete);

    while (1) {
        // Clean up zombie processes before new command
        cleanup_zombies();
        update_jobs();
        
        // Check for control structures first
        if (is_control_structure(NULL)) {
            if_block_t* if_block = parse_if_structure();
            if (if_block != NULL) {
                execute_if_block(if_block);
                
                // Free if_block memory
                free(if_block->condition_command);
                for (int i = 0; i < if_block->then_count; i++) {
                    free(if_block->then_commands[i]);
                }
                for (int i = 0; i < if_block->else_count; i++) {
                    free(if_block->else_commands[i]);
                }
                free(if_block);
                continue;
            }
        }
        
        // Regular command input
        cmdline = read_cmd(PROMPT);
        if (cmdline == NULL) break; // Ctrl+D
        
        // Handle history execution before tokenization
        if (cmdline[0] == '!') {
            handle_history_execution(&cmdline);
            if (cmdline == NULL) {
                free(cmdline);
                continue;
            }
        }
        
        // Check for command chaining (semicolon)
        if (strstr(cmdline, ";") != NULL) {
            execute_command_chain(cmdline);
        } else {
            // Parse the command line (handles pipes and redirection)
            pipeline = parse_command_line(cmdline);
            
            if (pipeline != NULL && pipeline->num_commands > 0) {
                // Check if it's a built-in command (only for single commands without pipes/redirection)
                if (pipeline->num_commands == 1 && 
                    pipeline->commands[0].input_file == NULL && 
                    pipeline->commands[0].output_file == NULL &&
                    !pipeline->commands[0].background) {
                    
                    char** arglist = pipeline->commands[0].args;
                    if (!handle_builtin(arglist)) {
                        execute_pipeline(pipeline);
                    }
                } else {
                    // Execute pipeline or command with redirection/background
                    execute_pipeline(pipeline);
                }
                
                free_pipeline(pipeline);
            }
        }
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
