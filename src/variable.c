#include "shell.h"

// Global variable list
variable_t variable_list[MAX_VARIABLES];
int variable_count = 0;

// Initialize variables
void init_variables() {
    for (int i = 0; i < MAX_VARIABLES; i++) {
        variable_list[i].name = NULL;
        variable_list[i].value = NULL;
    }
    variable_count = 0;
    
    // Set some default environment variables
    char* home = getenv("HOME");
    if (home) set_variable("HOME", home);
    
    char* user = getenv("USER");
    if (user) set_variable("USER", user);
    
    char* pwd = getenv("PWD");
    if (pwd) set_variable("PWD", pwd);
    
    set_variable("SHELL", "myshell");
}

// Set a variable
int set_variable(char* name, char* value) {
    if (name == NULL || value == NULL) return -1;
    
    // Check if variable already exists
    for (int i = 0; i < variable_count; i++) {
        if (variable_list[i].name != NULL && strcmp(variable_list[i].name, name) == 0) {
            // Update existing variable
            free(variable_list[i].value);
            variable_list[i].value = strdup(value);
            return 0;
        }
    }
    
    // Add new variable
    if (variable_count < MAX_VARIABLES) {
        variable_list[variable_count].name = strdup(name);
        variable_list[variable_count].value = strdup(value);
        variable_count++;
        return 0;
    }
    
    return -1; // No space
}

// Get a variable value
char* get_variable(char* name) {
    if (name == NULL) return NULL;
    
    for (int i = 0; i < variable_count; i++) {
        if (variable_list[i].name != NULL && strcmp(variable_list[i].name, name) == 0) {
            return variable_list[i].value;
        }
    }
    
    // Also check environment variables
    return getenv(name);
}

// Expand variables in argument list
void expand_variables(char*** arglist_ptr) {
    if (arglist_ptr == NULL || *arglist_ptr == NULL) return;
    
    char** arglist = *arglist_ptr;
    
    for (int i = 0; arglist[i] != NULL; i++) {
        char* arg = arglist[i];
        
        // Skip if argument is NULL or empty
        if (arg == NULL || arg[0] == '\0') continue;
        
        // Check if argument starts with $ and has more characters
        if (arg[0] == '$' && arg[1] != '\0') {
            char* var_name = arg + 1; // Skip the $
            char* var_value = get_variable(var_name);
            
            if (var_value != NULL) {
                // Replace the argument with the variable value
                free(arglist[i]);
                arglist[i] = strdup(var_value);
            } else {
                // Variable not found, replace with empty string
                free(arglist[i]);
                arglist[i] = strdup("");
            }
        }
    }
}

// Print all variables
void print_variables() {
    printf("Shell variables:\n");
    int shell_vars_printed = 0;
    
    for (int i = 0; i < variable_count; i++) {
        if (variable_list[i].name != NULL && variable_list[i].value != NULL) {
            printf("  %s=%s\n", variable_list[i].name, variable_list[i].value);
            shell_vars_printed++;
        }
    }
    
    if (shell_vars_printed == 0) {
        printf("  (no shell variables set)\n");
    }
    
    // Also print some important environment variables
    printf("\nEnvironment variables:\n");
    char* env_vars[] = {"PATH", "HOME", "USER", "SHELL", "PWD", NULL};
    int env_vars_printed = 0;
    
    for (int i = 0; env_vars[i] != NULL; i++) {
        char* value = getenv(env_vars[i]);
        if (value) {
            printf("  %s=%s\n", env_vars[i], value);
            env_vars_printed++;
        }
    }
    
    if (env_vars_printed == 0) {
        printf("  (no environment variables)\n");
    }
}

// Check if token is a variable assignment
int is_variable_assignment(char* token) {
    if (token == NULL) return 0;
    
    // Look for = sign that's not at the beginning
    char* equals = strchr(token, '=');
    if (equals == NULL || equals == token) return 0;
    
    // Check that there are no spaces around the =
    // and that the part before = is a valid variable name
    char* name_part = token;
    int name_len = equals - name_part;
    
    // Variable name must start with letter or underscore
    if (!((name_part[0] >= 'a' && name_part[0] <= 'z') ||
          (name_part[0] >= 'A' && name_part[0] <= 'Z') ||
          name_part[0] == '_')) {
        return 0;
    }
    
    // Check rest of variable name
    for (int i = 1; i < name_len; i++) {
        if (!((name_part[i] >= 'a' && name_part[i] <= 'z') ||
              (name_part[i] >= 'A' && name_part[i] <= 'Z') ||
              (name_part[i] >= '0' && name_part[i] <= '9') ||
              name_part[i] == '_')) {
            return 0;
        }
    }
    
    return 1;
}

// Handle variable assignment
int handle_variable_assignment(char* assignment) {
    if (!is_variable_assignment(assignment)) return 0;
    
    char* equals = strchr(assignment, '=');
    if (equals == NULL) return 0;
    
    // Split into name and value
    int name_len = equals - assignment;
    char* name = (char*)malloc(name_len + 1);
    strncpy(name, assignment, name_len);
    name[name_len] = '\0';
    
    char* value = equals + 1;
    
    // Handle quoted values
    if (value[0] == '"' || value[0] == '\'') {
        char quote = value[0];
        char* end_quote = strchr(value + 1, quote);
        if (end_quote != NULL) {
            *end_quote = '\0';
            value = value + 1;
        }
    }
    
    int result = set_variable(name, value);
    free(name);
    return result;
}
