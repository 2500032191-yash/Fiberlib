#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "fiber.h"
#include "memory.h"
#include <stdio.h>
#include <string.h>

static int g_pass_count = 0;
static int g_fail_count = 0;

static void assert_test(int condition, const char *test_name) {
    if (condition) {
        printf("[PASS] %s\n", test_name);
        g_pass_count++;
    } else {
        printf("[FAIL] %s\n", test_name);
        g_fail_count++;
    }
}

static void dummy_fiber_func(void *arg) {
    (void)arg;
}

static void test_fiber_creation(void) {
    fiber_t *f = fiber_create(dummy_fiber_func, NULL, 0);
    assert_test(f != NULL, "fiber_create allocates fiber struct");
    if (f) {
        assert_test(f->id > 0, "fiber_create assigns positive fiber ID");
        assert_test(f->state == FIBER_CREATED, "fiber_create sets initial state to FIBER_CREATED");
        assert_test(f->stack != NULL, "fiber_create allocates non-null stack");
        fiber_destroy(f);
    }
}

static void test_fiber_state_string(void) {
    assert_test(strcmp(fiber_state_to_string(FIBER_CREATED), "CREATED") == 0, "fiber_state_to_string CREATED");
    assert_test(strcmp(fiber_state_to_string(FIBER_READY), "READY") == 0, "fiber_state_to_string READY");
    assert_test(strcmp(fiber_state_to_string(FIBER_RUNNING), "RUNNING") == 0, "fiber_state_to_string RUNNING");
    assert_test(strcmp(fiber_state_to_string(FIBER_BLOCKED), "BLOCKED") == 0, "fiber_state_to_string BLOCKED");
    assert_test(strcmp(fiber_state_to_string(FIBER_TERMINATED), "TERMINATED") == 0, "fiber_state_to_string TERMINATED");
}

int main(void) {
    printf("=========================================\n");
    printf(" UNIT TEST SUITE: fiber.c\n");
    printf("=========================================\n");

    test_fiber_creation();
    test_fiber_state_string();

    printf("\nTest Summary: %d Passed, %d Failed\n", g_pass_count, g_fail_count);
    return g_fail_count == 0 ? 0 : 1;
}
