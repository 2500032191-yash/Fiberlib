#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(void) {
    printf("=========================================\n");
    printf(" DEMO 6: Memory Layout (Stack, Heap, mmap)\n");
    printf("=========================================\n");

    int stack_var = 42;
    printf("[MEMORY DEMO] Stack Variable Address: %p\n", (void *)&stack_var);

    int *heap_ptr = (int *)fiber_malloc(sizeof(int) * 100);
    printf("[MEMORY DEMO] Heap Allocation Address: %p (Size: %zu bytes)\n", (void *)heap_ptr, sizeof(int) * 100);

    size_t mmap_size = 64 * 1024;
    void *mmap_ptr = fiber_stack_alloc(mmap_size);
    printf("[MEMORY DEMO] mmap Stack Allocation Address: %p (Size: %zu bytes)\n", mmap_ptr, mmap_size);

    printf("\n[MEMORY DEMO] Address Space Layout Comparison:\n");
    printf("  Stack Address: %p\n", (void *)&stack_var);
    printf("  Heap Address:  %p\n", (void *)heap_ptr);
    printf("  mmap Address:  %p\n\n", mmap_ptr);

    memory_tracker_print_stats();

    printf("[MEMORY DEMO] Cleaning up allocated heap and mmap regions...\n");
    fiber_free(heap_ptr);
    fiber_stack_free(mmap_ptr, mmap_size);

    memory_tracker_print_stats();

    printf("[MEMORY DEMO] Memory Demo Completed Successfully.\n\n");
    return 0;
}
