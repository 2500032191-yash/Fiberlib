#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

static memory_stats_t g_mem_stats = {0, 0, 0, 0};

void *fiber_stack_alloc(size_t size) {
    if (size == 0) {
        size = FIBER_DEFAULT_STACK_SIZE;
    }
    
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size > 0 && (size % (size_t)page_size) != 0) {
        size = (((size / (size_t)page_size) + 1) * (size_t)page_size);
    }

    void *stack = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (stack == MAP_FAILED) {
        perror("fiber_stack_alloc: mmap failed");
        return NULL;
    }

    g_mem_stats.total_allocated += size;
    g_mem_stats.active_allocations += 1;
    g_mem_stats.stack_allocations += 1;

    return stack;
}

void fiber_stack_free(void *stack, size_t size) {
    if (!stack || size == 0) return;

    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size > 0 && (size % (size_t)page_size) != 0) {
        size = (((size / (size_t)page_size) + 1) * (size_t)page_size);
    }

    if (munmap(stack, size) == 0) {
        g_mem_stats.total_freed += size;
        if (g_mem_stats.active_allocations > 0) g_mem_stats.active_allocations -= 1;
        if (g_mem_stats.stack_allocations > 0) g_mem_stats.stack_allocations -= 1;
    } else {
        perror("fiber_stack_free: munmap failed");
    }
}

void *fiber_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr) {
        g_mem_stats.total_allocated += size;
        g_mem_stats.active_allocations += 1;
    }
    return ptr;
}

void fiber_free(void *ptr) {
    if (!ptr) return;
    free(ptr);
    if (g_mem_stats.active_allocations > 0) {
        g_mem_stats.active_allocations -= 1;
    }
}

memory_stats_t memory_get_stats(void) {
    return g_mem_stats;
}

size_t memory_get_total_allocated(void) {
    return g_mem_stats.total_allocated;
}

void memory_tracker_print_stats(void) {
    printf("[MEMORY TRACKER] Total Allocated: %zu bytes | Active Allocations: %zu | Stacks: %zu | Total Freed: %zu bytes\n",
           g_mem_stats.total_allocated,
           g_mem_stats.active_allocations,
           g_mem_stats.stack_allocations,
           g_mem_stats.total_freed);
}
