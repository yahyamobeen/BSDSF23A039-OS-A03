// Read multiline command for control structures
char* read_multiline_cmd(char* prompt) {
    printf("%s", prompt);
    
    char* cmdline = (char*)malloc(MAX_LEN);
    char buffer[MAX_LEN];
    int total_len = 0;
    
    cmdline[0] = '\0';
    
    while (fgets(buffer, MAX_LEN, stdin) != NULL) {
        // Remove newline
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // Check for end of control structure
        if (strcmp(buffer, "fi") == 0) {
            break;
        }
        
        // Append to command line
        if (total_len + strlen(buffer) + 1 < MAX_LEN) {
            if (total_len > 0) {
                strcat(cmdline, ";");
                total_len++;
            }
            strcat(cmdline, buffer);
            total_len += strlen(buffer);
        }
        
        printf("> ");
    }
    
    if (total_len == 0) {
        free(cmdline);
        return NULL;
    }
    
    return cmdline;
}
