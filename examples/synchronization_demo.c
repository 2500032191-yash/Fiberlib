#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "fiber.h"
#include "scheduler.h"
#include "sync.h"
#include <stdio.h>
#include <stdint.h>

static int g_shared_counter_unsafe = 0;
static int g_shared_counter_safe = 0;
static fiber_mutex_t g_mutex;
static fiber_sem_t g_sem;

static void unsafe_increment_fiber(void *arg) {
    uintptr_t id = (uintptr_t)arg;
    for (int i = 0; i < 5; i++) {
        int temp = g_shared_counter_unsafe;
        printf("[UNSAFE INCREMENT] Fiber #%lu read counter = %d\n", id, temp);
        fiber_yield();
        temp = temp + 1;
        g_shared_counter_unsafe = temp;
        printf("[UNSAFE INCREMENT] Fiber #%lu updated counter = %d\n", id, temp);
    }
}

static void safe_increment_fiber(void *arg) {
    uintptr_t id = (uintptr_t)arg;
    for (int i = 0; i < 5; i++) {
        printf("[MUTEX PROTECTED] Fiber #%lu requesting mutex lock...\n", id);
        fiber_mutex_lock(&g_mutex);
        printf("[MUTEX PROTECTED] Fiber #%lu acquired mutex lock.\n", id);
        
        int temp = g_shared_counter_safe;
        temp = temp + 1;
        g_shared_counter_safe = temp;
        printf("[MUTEX PROTECTED] Fiber #%lu updated protected counter = %d\n", id, g_shared_counter_safe);
        
        printf("[MUTEX PROTECTED] Fiber #%lu releasing mutex lock.\n", id);
        fiber_mutex_unlock(&g_mutex);
        fiber_yield();
    }
}

static void semaphore_waiter_fiber(void *arg) {
    (void)arg;
    printf("[SEMAPHORE DEMO] Waiter Fiber waiting on semaphore (sem_wait)...\n");
    fiber_sem_wait(&g_sem);
    printf("[SEMAPHORE DEMO] Waiter Fiber ACQUIRED semaphore token!\n");
}

static void semaphore_poster_fiber(void *arg) {
    (void)arg;
    printf("[SEMAPHORE DEMO] Poster Fiber yielding first...\n");
    fiber_yield();
    printf("[SEMAPHORE DEMO] Poster Fiber posting semaphore token (sem_post)...\n");
    fiber_sem_post(&g_sem);
}

int main(void) {
    printf("=========================================\n");
    printf(" DEMO 8: Fiber Synchronization & Deadlock Detection\n");
    printf("=========================================\n");

    // PART 1: Unsafe (Simulated Race Window)
    printf("\n--- PART 1: Unsafe Shared Data Access ---\n");
    scheduler_init();
    g_shared_counter_unsafe = 0;
    scheduler_spawn(unsafe_increment_fiber, (void *)1);
    scheduler_spawn(unsafe_increment_fiber, (void *)2);
    scheduler_run();
    printf("[SYNC DEMO] Final Unsafe Counter Value: %d\n", g_shared_counter_unsafe);
    scheduler_shutdown();

    // PART 2: Safe Mutex Protection
    printf("\n--- PART 2: Mutex Protected Data Access ---\n");
    scheduler_init();
    fiber_mutex_init(&g_mutex);
    g_shared_counter_safe = 0;
    scheduler_spawn(safe_increment_fiber, (void *)1);
    scheduler_spawn(safe_increment_fiber, (void *)2);
    scheduler_run();
    printf("[SYNC DEMO] Final Protected Counter Value: %d (Expected 10)\n", g_shared_counter_safe);
    fiber_mutex_destroy(&g_mutex);
    scheduler_shutdown();

    // PART 3: Semaphore Signaling
    printf("\n--- PART 3: Semaphore Signaling (Initial Val: 0) ---\n");
    scheduler_init();
    fiber_sem_init(&g_sem, 0);
    scheduler_spawn(semaphore_waiter_fiber, NULL);
    scheduler_spawn(semaphore_poster_fiber, NULL);
    scheduler_run();
    fiber_sem_destroy(&g_sem);
    scheduler_shutdown();

    // PART 4: Deadlock Detection Logging Check
    printf("\n--- PART 4: Deadlock Detection Logging Check ---\n");
    scheduler_init();
    fiber_sync_check_deadlock();
    scheduler_shutdown();

    printf("\n[SYNC DEMO] Synchronization Demo Completed Successfully.\n\n");
    return 0;
}
