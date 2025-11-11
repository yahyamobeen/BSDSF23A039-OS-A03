#include "shell.h"

// This file is now mostly redundant as functionality moved to shell.c
// Keeping it for backward compatibility

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
