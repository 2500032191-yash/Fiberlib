#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "fiber.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdint.h>

static void worker_fiber(void *arg) {
    uintptr_t fiber_num = (uintptr_t)arg;
    fiber_t *curr = fiber_current();

    for (int i = 1; i <= 3; i++) {
        printf("[SCHEDULER DEMO] Fiber #%lu (ID: %lu) - Round Iteration %d/3\n",
               fiber_num, fiber_get_id(curr), i);
        printf("[SCHEDULER DEMO] Fiber #%lu Yielding control to next fiber in queue...\n", fiber_num);
        fiber_yield();
    }
    printf("[SCHEDULER DEMO] Fiber #%lu Completed all iterations and terminating.\n", fiber_num);
}

int main(void) {
    printf("=========================================\n");
    printf(" DEMO 2: Round-Robin Queue Rotation\n");
    printf("=========================================\n");

    scheduler_init();

    printf("[MAIN] Spawning 4 Fibers for Round-Robin Scheduling...\n");
    for (uintptr_t i = 1; i <= 4; i++) {
        scheduler_spawn(worker_fiber, (void *)i);
    }

    printf("[MAIN] Starting Scheduler Loop...\n");
    scheduler_run();

    printf("[MAIN] All Fibers executed in Round-Robin order.\n");
    scheduler_shutdown();
    printf("[MAIN] Scheduling Demo Completed Successfully.\n\n");
    return 0;
}
