#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    printf("=========================================\n");
    printf(" DEMO 3: POSIX Process Creation (fork/waitpid)\n");
    printf("=========================================\n");

    pid_t parent_pid = getpid();
    printf("[PROCESS DEMO] Parent Process PID: %d. Forking child process...\n", parent_pid);

    pid_t pid = fork();

    if (pid < 0) {
        perror("[PROCESS DEMO] fork failed");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // Child process
        pid_t child_pid = getpid();
        pid_t child_ppid = getppid();
        printf("[CHILD PROCESS] Hello from Child Process! Child PID: %d | Parent PID: %d\n",
               child_pid, child_ppid);
        printf("[CHILD PROCESS] Performing simulated work in child process...\n");
        usleep(50000);
        printf("[CHILD PROCESS] Exiting child process with status code 42...\n");
        exit(42);
    } else {
        // Parent process
        printf("[PARENT PROCESS] Spawned Child Process with PID: %d. Waiting for termination...\n", pid);
        int status = 0;
        pid_t waited_pid = waitpid(pid, &status, 0);

        if (waited_pid > 0) {
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                printf("[PARENT PROCESS] Child PID %d exited normally with exit status: %d\n",
                       waited_pid, exit_code);
            } else if (WIFSIGNALED(status)) {
                printf("[PARENT PROCESS] Child PID %d killed by signal: %d\n",
                       waited_pid, WTERMSIG(status));
            }
        }
    }

    printf("[PROCESS DEMO] Process Demo Completed Successfully.\n\n");
    return EXIT_SUCCESS;
}
