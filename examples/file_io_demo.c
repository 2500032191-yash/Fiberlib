#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void) {
    printf("=========================================\n");
    printf(" DEMO 7: Low-Level File Descriptor Operations\n");
    printf("=========================================\n");

    const char *filename = "fiber_io_test.tmp";
    const char *write_data = "FiberLib Low-Level POSIX File Descriptor I/O Test String.\nLine 2: Green threads + POSIX syscalls.";

    printf("[FILE IO] Creating and opening file '%s' (O_CREAT | O_WRONLY | O_TRUNC)...\n", filename);
    int fd_write = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd_write < 0) {
        perror("[FILE IO] open for write failed");
        return EXIT_FAILURE;
    }
    printf("[FILE IO] File Descriptor Opened for Write: FD = %d\n", fd_write);

    ssize_t bytes_written = write(fd_write, write_data, strlen(write_data));
    if (bytes_written < 0) {
        perror("[FILE IO] write failed");
        close(fd_write);
        return EXIT_FAILURE;
    }
    printf("[FILE IO] Successfully wrote %zd bytes to FD %d\n", bytes_written, fd_write);
    close(fd_write);
    printf("[FILE IO] Closed Write File Descriptor FD = %d\n", fd_write);

    printf("[FILE IO] Re-opening file '%s' for Read (O_RDONLY)...\n", filename);
    int fd_read = open(filename, O_RDONLY);
    if (fd_read < 0) {
        perror("[FILE IO] open for read failed");
        return EXIT_FAILURE;
    }
    printf("[FILE IO] File Descriptor Opened for Read: FD = %d\n", fd_read);

    char buffer[512];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = read(fd_read, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        perror("[FILE IO] read failed");
        close(fd_read);
        return EXIT_FAILURE;
    }
    printf("[FILE IO] Read %zd bytes from FD %d:\n", bytes_read, fd_read);
    printf("-----------------------------------------\n");
    printf("%s\n", buffer);
    printf("-----------------------------------------\n");

    close(fd_read);
    printf("[FILE IO] Closed Read File Descriptor FD = %d\n", fd_read);

    unlink(filename);
    printf("[FILE IO] Unlinked temporary test file '%s'\n", filename);
    printf("[FILE IO] File I/O Demo Completed Successfully.\n\n");
    return EXIT_SUCCESS;
}
