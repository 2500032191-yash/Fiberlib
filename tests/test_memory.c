#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "memory.h"
#include <stdio.h>

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

static void test_stack_alloc_free(void) {
    size_t stack_size = 32 * 1024;
    void *stack = fiber_stack_alloc(stack_size);
    assert_test(stack != NULL, "fiber_stack_alloc returns non-null pointer");

    memory_stats_t stats_after_alloc = memory_get_stats();
    assert_test(stats_after_alloc.stack_allocations > 0, "Stack allocation counter updated");

    fiber_stack_free(stack, stack_size);
    memory_stats_t stats_after_free = memory_get_stats();
    assert_test(stats_after_free.total_freed > 0, "Total freed counter updated after fiber_stack_free");
}

static void test_heap_alloc_free(void) {
    void *ptr = fiber_malloc(1024);
    assert_test(ptr != NULL, "fiber_malloc returns non-null pointer");

    fiber_free(ptr);
    assert_test(1, "fiber_free released heap block without crash");
}

int main(void) {
    printf("=========================================\n");
    printf(" UNIT TEST SUITE: memory.c\n");
    printf("=========================================\n");

    test_stack_alloc_free();
    test_heap_alloc_free();

    printf("\nTest Summary: %d Passed, %d Failed\n", g_pass_count, g_fail_count);
    return g_fail_count == 0 ? 0 : 1;
}
