#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "fiber.h"
#include <stddef.h>

typedef struct {
    fiber_t *ready_head;
    fiber_t *ready_tail;
    fiber_t *current_fiber;
    fiber_t *main_fiber;
    size_t active_fibers;
    int is_running;
} scheduler_t;

int scheduler_init(void);
void scheduler_shutdown(void);
int scheduler_spawn(fiber_func_t func, void *arg);
int scheduler_spawn_fiber(fiber_t *fiber);
void scheduler_run(void);
void scheduler_yield(void);
void scheduler_block_current(fiber_state_t reason);
void scheduler_unblock(fiber_t *fiber);
fiber_t *scheduler_get_current_fiber(void);
size_t scheduler_get_active_count(void);
scheduler_t *scheduler_get_instance(void);

#endif /* SCHEDULER_H */
