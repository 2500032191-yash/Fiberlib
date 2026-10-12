#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    printf("=========================================\n");
    printf(" DEMO 4: Inter-Process Communication via Anonymous Pipe\n");
    printf("=========================================\n");

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("[IPC DEMO] pipe failed");
        return EXIT_FAILURE;
    }

    printf("[IPC DEMO] Anonymous Pipe Created: Read FD = %d | Write FD = %d\n", pipefd[0], pipefd[1]);

    pid_t pid = fork();

    if (pid < 0) {
        perror("[IPC DEMO] fork failed");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        // Child Process: Reads from pipe
        close(pipefd[1]); // Close unused write end

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));

        printf("[CHILD IPC] Child PID %d waiting to read message from pipe...\n", getpid());
        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            printf("[CHILD IPC] Successfully received %zd bytes from Parent!\n", bytes_read);
            printf("[CHILD IPC] Message Content: \"%s\"\n", buffer);
        } else {
            perror("[CHILD IPC] read failed");
        }

        close(pipefd[0]);
        exit(0);
    } else {
        // Parent Process: Writes to pipe
        close(pipefd[0]); // Close unused read end

        const char *message = "Greetings from Parent Process via POSIX Anonymous Pipe!";
        printf("[PARENT IPC] Parent PID %d sending message to Child...\n", getpid());
        printf("[PARENT IPC] Writing Message: \"%s\"\n", message);

        ssize_t bytes_written = write(pipefd[1], message, strlen(message) + 1);
        if (bytes_written > 0) {
            printf("[PARENT IPC] Wrote %zd bytes to pipe.\n", bytes_written);
        }

        close(pipefd[1]);

        int status;
        waitpid(pid, &status, 0);
        printf("[PARENT IPC] Child process completed.\n");
    }

    printf("[IPC DEMO] IPC Demo Completed Successfully.\n\n");
    return EXIT_SUCCESS;
}
