#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "fiber.h"
#include "scheduler.h"
#include <stdio.h>

static void fiber_alpha(void *arg) {
    (void)arg;
    fiber_t *curr = fiber_current();
    printf("[BASIC FIBERS] Fiber %lu (Alpha) STARTED. State: %s\n",
           fiber_get_id(curr), fiber_state_to_string(curr->state));
    
    printf("[BASIC FIBERS] Fiber %lu (Alpha) Yielding to Fiber Beta...\n", fiber_get_id(curr));
    fiber_yield();

    printf("[BASIC FIBERS] Fiber %lu (Alpha) RESUMED. Completing work...\n", fiber_get_id(curr));
    printf("[BASIC FIBERS] Fiber %lu (Alpha) TERMINATING CLEANLY.\n", fiber_get_id(curr));
}

static void fiber_beta(void *arg) {
    (void)arg;
    fiber_t *curr = fiber_current();
    printf("[BASIC FIBERS] Fiber %lu (Beta) STARTED. State: %s\n",
           fiber_get_id(curr), fiber_state_to_string(curr->state));
    
    printf("[BASIC FIBERS] Fiber %lu (Beta) Yielding back to Fiber Alpha...\n", fiber_get_id(curr));
    fiber_yield();

    printf("[BASIC FIBERS] Fiber %lu (Beta) RESUMED. Completing work...\n", fiber_get_id(curr));
    printf("[BASIC FIBERS] Fiber %lu (Beta) TERMINATING CLEANLY.\n", fiber_get_id(curr));
}

int main(void) {
    printf("=========================================\n");
    printf(" DEMO 1: Basic Fibers & Yielding\n");
    printf("=========================================\n");

    scheduler_init();

    printf("[MAIN] Spawning Fiber Alpha & Fiber Beta...\n");
    scheduler_spawn(fiber_alpha, NULL);
    scheduler_spawn(fiber_beta, NULL);

    printf("[MAIN] Starting Scheduler Loop...\n");
    scheduler_run();

    printf("[MAIN] Scheduler finished. Cleaning up...\n");
    scheduler_shutdown();
    printf("[MAIN] Basic Fibers Demo Completed Successfully.\n\n");
    return 0;
}
