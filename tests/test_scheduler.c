#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "fiber.h"
#include "scheduler.h"
#include <stdio.h>

static int g_pass_count = 0;
static int g_fail_count = 0;
static int g_execution_sequence[10];
static int g_seq_index = 0;

static void assert_test(int condition, const char *test_name) {
    if (condition) {
        printf("[PASS] %s\n", test_name);
        g_pass_count++;
    } else {
        printf("[FAIL] %s\n", test_name);
        g_fail_count++;
    }
}

static void seq_fiber_1(void *arg) {
    (void)arg;
    g_execution_sequence[g_seq_index++] = 1;
    fiber_yield();
    g_execution_sequence[g_seq_index++] = 3;
}

static void seq_fiber_2(void *arg) {
    (void)arg;
    g_execution_sequence[g_seq_index++] = 2;
    fiber_yield();
    g_execution_sequence[g_seq_index++] = 4;
}

static void test_scheduler_lifecycle(void) {
    int init_res = scheduler_init();
    assert_test(init_res == 0, "scheduler_init returns 0");
    assert_test(scheduler_get_active_count() == 0, "Initial active fiber count is 0");
    scheduler_shutdown();
}

static void test_scheduler_execution_order(void) {
    scheduler_init();
    g_seq_index = 0;

    scheduler_spawn(seq_fiber_1, NULL);
    scheduler_spawn(seq_fiber_2, NULL);

    assert_test(scheduler_get_active_count() == 2, "Active fiber count is 2 after spawning 2 fibers");

    scheduler_run();

    assert_test(g_seq_index == 4, "All sequence steps executed");
    assert_test(g_execution_sequence[0] == 1, "Fiber 1 step 1 executed first");
    assert_test(g_execution_sequence[1] == 2, "Fiber 2 step 1 executed second");
    assert_test(g_execution_sequence[2] == 3, "Fiber 1 step 2 executed third");
    assert_test(g_execution_sequence[3] == 4, "Fiber 2 step 2 executed fourth");

    assert_test(scheduler_get_active_count() == 0, "Active fiber count is 0 after completion");
    scheduler_shutdown();
}

int main(void) {
    printf("=========================================\n");
    printf(" UNIT TEST SUITE: scheduler.c\n");
    printf("=========================================\n");

    test_scheduler_lifecycle();
    test_scheduler_execution_order();

    printf("\nTest Summary: %d Passed, %d Failed\n", g_pass_count, g_fail_count);
    return g_fail_count == 0 ? 0 : 1;
}
