#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "sync.h"
#include "scheduler.h"
#include <stdio.h>

static int g_pass_count = 0;
static int g_fail_count = 0;
static fiber_mutex_t g_test_mutex;
static fiber_sem_t g_test_sem;
static int g_sync_counter = 0;

static void assert_test(int condition, const char *test_name) {
    if (condition) {
        printf("[PASS] %s\n", test_name);
        g_pass_count++;
    } else {
        printf("[FAIL] %s\n", test_name);
        g_fail_count++;
    }
}

static void test_mutex_basic(void) {
    fiber_mutex_init(&g_test_mutex);
    assert_test(g_test_mutex.locked == 0, "Mutex initially unlocked");

    int try_res = fiber_mutex_trylock(&g_test_mutex);
    assert_test(try_res == 0, "fiber_mutex_trylock succeeds on unlocked mutex");
    assert_test(g_test_mutex.locked == 1, "Mutex locked state is 1");

    int try_res2 = fiber_mutex_trylock(&g_test_mutex);
    assert_test(try_res2 == -1, "fiber_mutex_trylock fails on locked mutex");

    fiber_mutex_unlock(&g_test_mutex);
    assert_test(g_test_mutex.locked == 0, "Mutex unlocked after fiber_mutex_unlock");

    fiber_mutex_destroy(&g_test_mutex);
}

static void mutex_fiber_worker(void *arg) {
    (void)arg;
    fiber_mutex_lock(&g_test_mutex);
    g_sync_counter++;
    fiber_yield();
    g_sync_counter++;
    fiber_mutex_unlock(&g_test_mutex);
}

static void test_mutex_concurrency(void) {
    scheduler_init();
    fiber_mutex_init(&g_test_mutex);
    g_sync_counter = 0;

    scheduler_spawn(mutex_fiber_worker, NULL);
    scheduler_spawn(mutex_fiber_worker, NULL);

    scheduler_run();

    assert_test(g_sync_counter == 4, "Mutex concurrency test counter equals 4");
    fiber_mutex_destroy(&g_test_mutex);
    scheduler_shutdown();
}

static void test_sem_basic(void) {
    fiber_sem_init(&g_test_sem, 1);
    assert_test(g_test_sem.value == 1, "Semaphore initial value is 1");

    fiber_sem_wait(&g_test_sem);
    assert_test(g_test_sem.value == 0, "Semaphore value is 0 after wait");

    fiber_sem_post(&g_test_sem);
    assert_test(g_test_sem.value == 1, "Semaphore value is 1 after post");

    fiber_sem_destroy(&g_test_sem);
}

int main(void) {
    printf("=========================================\n");
    printf(" UNIT TEST SUITE: sync.c\n");
    printf("=========================================\n");

    test_mutex_basic();
    test_mutex_concurrency();
    test_sem_basic();

    printf("\nTest Summary: %d Passed, %d Failed\n", g_pass_count, g_fail_count);
    return g_fail_count == 0 ? 0 : 1;
}
