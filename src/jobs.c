#include "shell.h"

// Global job list
job_t job_list[MAX_JOBS];
int job_count = 0;

// Initialize job list
void init_jobs() {
    for (int i = 0; i < MAX_JOBS; i++) {
        job_list[i].pid = -1;
        job_list[i].command = NULL;
        job_list[i].job_id = -1;
        job_list[i].status = -1;
    }
    job_count = 0;
}

// Add a new background job
void add_job(pid_t pid, char* command) {
    if (job_count >= MAX_JOBS) {
        printf("Job list full. Cannot add more jobs.\n");
        return;
    }
    
    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_list[i].pid == -1) {
            job_list[i].pid = pid;
            job_list[i].command = strdup(command);
            job_list[i].job_id = i + 1;
            job_list[i].status = 0; // running
            job_count++;
            printf("[%d] %d\n", job_list[i].job_id, pid);
            break;
        }
    }
}

// Remove a job by PID
void remove_job(pid_t pid) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_list[i].pid == pid) {
            if (job_list[i].command != NULL) {
                free(job_list[i].command);
            }
            job_list[i].pid = -1;
            job_list[i].command = NULL;
            job_list[i].job_id = -1;
            job_list[i].status = -1;
            job_count--;
            break;
        }
    }
}

// Update job status and remove completed jobs
void update_jobs() {
    int status;
    pid_t pid;
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < MAX_JOBS; i++) {
            if (job_list[i].pid == pid) {
                if (WIFEXITED(status)) {
                    printf("[%d] Done %s\n", job_list[i].job_id, job_list[i].command);
                } else if (WIFSIGNALED(status)) {
                    printf("[%d] Killed %s\n", job_list[i].job_id, job_list[i].command);
                }
                remove_job(pid);
                break;
            }
        }
    }
}

// Print all active jobs
void print_jobs() {
    if (job_count == 0) {
        printf("No active jobs.\n");
        return;
    }
    
    printf("Active jobs:\n");
    for (int i = 0; i < MAX_JOBS; i++) {
        if (job_list[i].pid != -1) {
            printf("[%d] %d %s", job_list[i].job_id, job_list[i].pid, job_list[i].command);
            if (job_list[i].status == 0) {
                printf(" (running)");
            } else if (job_list[i].status == 1) {
                printf(" (completed)");
            } else if (job_list[i].status == 2) {
                printf(" (stopped)");
            }
            printf("\n");
        }
    }
}

// Clean up zombie processes
void cleanup_zombies() {
    int status;
    pid_t pid;
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Zombie process reaped, no need to print anything
    }
}
