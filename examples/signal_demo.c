#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

static volatile sig_atomic_t g_signal_received = 0;
static volatile sig_atomic_t g_last_signal = 0;

static void handle_signal(int sig) {
    g_signal_received = 1;
    g_last_signal = sig;
}

int main(void) {
    printf("=========================================\n");
    printf(" DEMO 5: POSIX Signal Handling & Async Notifications\n");
    printf("=========================================\n");

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("[SIGNAL DEMO] sigaction SIGUSR1 failed");
        return EXIT_FAILURE;
    }
    printf("[SIGNAL DEMO] Registered sigaction handler for SIGUSR1 (Signal #%d).\n", SIGUSR1);

    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("[SIGNAL DEMO] sigaction SIGUSR2 failed");
        return EXIT_FAILURE;
    }
    printf("[SIGNAL DEMO] Registered sigaction handler for SIGUSR2 (Signal #%d).\n", SIGUSR2);

    pid_t self_pid = getpid();
    printf("[SIGNAL DEMO] Process PID %d raising SIGUSR1 to self...\n", self_pid);
    kill(self_pid, SIGUSR1);

    if (g_signal_received) {
        printf("[SIGNAL NOTIFICATION] Async Signal Handled! Received Signal: %d (SIGUSR1)\n", g_last_signal);
        g_signal_received = 0;
    }

    printf("[SIGNAL DEMO] Process PID %d raising SIGUSR2 to self...\n", self_pid);
    kill(self_pid, SIGUSR2);

    if (g_signal_received) {
        printf("[SIGNAL NOTIFICATION] Async Signal Handled! Received Signal: %d (SIGUSR2)\n", g_last_signal);
        g_signal_received = 0;
    }

    printf("[SIGNAL DEMO] Signal Handling Demo Completed Successfully.\n\n");
    return EXIT_SUCCESS;
}
