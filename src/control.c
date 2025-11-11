#include "shell.h"

// Check if the next command is a control structure
int is_control_structure(char* cmdline) {
    // For now, we'll check by reading the first line
    // In a more advanced implementation, we'd parse the cmdline
    char* first_line = readline(""); // Read without prompt to check
    if (first_line == NULL) return 0;
    
    int is_control = 0;
    // Trim whitespace
    char* trimmed = first_line;
    while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
    
    if (strncmp(trimmed, "if", 2) == 0 && (trimmed[2] == ' ' || trimmed[2] == '\t' || trimmed[2] == '\0')) {
        is_control = 1;
    }
    
    // Put the line back for parsing
    if (is_control) {
        rl_replace_line(first_line, 0);
        rl_redisplay();
    }
    
    free(first_line);
    return is_control;
}

// Parse if-then-else-fi structure
if_block_t* parse_if_structure() {
    if_block_t* if_block = (if_block_t*)malloc(sizeof(if_block_t));
    if_block->then_count = 0;
    if_block->else_count = 0;
    if_block->has_else = 0;
    
    char* line;
    int in_then_block = 0;
    int in_else_block = 0;
    int block_ended = 0;
    
    // Read the if line
    line = readline("if> ");
    if (line == NULL) {
        free(if_block);
        return NULL;
    }
    
    // Extract condition command (remove "if" keyword)
    char* condition = line;
    while (*condition == ' ' || *condition == '\t') condition++;
    condition += 2; // Skip "if"
    while (*condition == ' ' || *condition == '\t') condition++;
    
    if_block->condition_command = strdup(condition);
    free(line);
    
    // Read then block
    in_then_block = 1;
    while (in_then_block && !block_ended) {
        line = readline("then> ");
        if (line == NULL) break;
        
        // Trim whitespace
        char* trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        
        if (strcmp(trimmed, "else") == 0) {
            in_then_block = 0;
            in_else_block = 1;
            if_block->has_else = 1;
        } else if (strcmp(trimmed, "fi") == 0) {
            in_then_block = 0;
            block_ended = 1;
        } else if (strlen(trimmed) > 0) {
            if (if_block->then_count < MAX_COMMANDS) {
                if_block->then_commands[if_block->then_count++] = strdup(trimmed);
            }
        }
        free(line);
    }
    
    // Read else block if present
    while (in_else_block && !block_ended) {
        line = readline("else> ");
        if (line == NULL) break;
        
        // Trim whitespace
        char* trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        
        if (strcmp(trimmed, "fi") == 0) {
            in_else_block = 0;
            block_ended = 1;
        } else if (strlen(trimmed) > 0) {
            if (if_block->else_count < MAX_COMMANDS) {
                if_block->else_commands[if_block->else_count++] = strdup(trimmed);
            }
        }
        free(line);
    }
    
    // If we didn't reach fi, read until we find it
    while (!block_ended) {
        line = readline("> ");
        if (line == NULL) break;
        
        // Trim whitespace
        char* trimmed = line;
        while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
        
        if (strcmp(trimmed, "fi") == 0) {
            block_ended = 1;
        }
        free(line);
    }
    
    return if_block;
}

// Execute a condition command and return its exit status
int execute_condition(char* condition) {
    if (condition == NULL || strlen(condition) == 0) return 1; // Empty condition fails
    
    pipeline_t* pipeline = parse_command_line(condition);
    if (pipeline == NULL || pipeline->num_commands == 0) return 1;
    
    int status = 0;
    pid_t pid = fork();
    
    if (pid == 0) {
        // Child process - execute the condition
        if (pipeline->num_commands == 1) {
            execute_single_command(&pipeline->commands[0]);
        } else {
            execute_pipeline(pipeline);
        }
        exit(0);
    } else {
        // Parent process - wait for condition result
        waitpid(pid, &status, 0);
        free_pipeline(pipeline);
        
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status); // Return the exit code
        } else {
            return 1; // If process was killed, consider it failure
        }
    }
}

// Execute if-then-else block
int execute_if_block(if_block_t* if_block) {
    if (if_block == NULL) return -1;
    
    // Execute the condition
    int condition_result = execute_condition(if_block->condition_command);
    
    if (condition_result == 0) {
        // Condition succeeded - execute then block
        for (int i = 0; i < if_block->then_count; i++) {
            if (if_block->then_commands[i] != NULL) {
                pipeline_t* pipeline = parse_command_line(if_block->then_commands[i]);
                if (pipeline != NULL) {
                    execute_pipeline(pipeline);
                    free_pipeline(pipeline);
                }
            }
        }
    } else if (if_block->has_else) {
        // Condition failed and else block exists - execute else block
        for (int i = 0; i < if_block->else_count; i++) {
            if (if_block->else_commands[i] != NULL) {
                pipeline_t* pipeline = parse_command_line(if_block->else_commands[i]);
                if (pipeline != NULL) {
                    execute_pipeline(pipeline);
                    free_pipeline(pipeline);
                }
            }
        }
    }
    
    return 0;
}
