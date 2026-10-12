#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

#define FIBER_DEFAULT_STACK_SIZE (64 * 1024)

typedef struct {
    size_t total_allocated;
    size_t active_allocations;
    size_t stack_allocations;
    size_t total_freed;
} memory_stats_t;

void *fiber_stack_alloc(size_t size);
void fiber_stack_free(void *stack, size_t size);
void *fiber_malloc(size_t size);
void fiber_free(void *ptr);
memory_stats_t memory_get_stats(void);
void memory_tracker_print_stats(void);
size_t memory_get_total_allocated(void);

#endif /* MEMORY_H */
