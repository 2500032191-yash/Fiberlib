#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "scheduler.h"
#include "context.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

static scheduler_t g_scheduler;

static void enqueue_ready(fiber_t *f) {
    if (!f) return;
    f->next = NULL;
    f->prev = g_scheduler.ready_tail;
    if (g_scheduler.ready_tail) {
        g_scheduler.ready_tail->next = f;
    } else {
        g_scheduler.ready_head = f;
    }
    g_scheduler.ready_tail = f;
}

static fiber_t *dequeue_ready(void) {
    if (!g_scheduler.ready_head) return NULL;
    fiber_t *f = g_scheduler.ready_head;
    g_scheduler.ready_head = f->next;
    if (g_scheduler.ready_head) {
        g_scheduler.ready_head->prev = NULL;
    } else {
        g_scheduler.ready_tail = NULL;
    }
    f->next = NULL;
    f->prev = NULL;
    return f;
}

int scheduler_init(void) {
    g_scheduler.ready_head = NULL;
    g_scheduler.ready_tail = NULL;
    g_scheduler.current_fiber = NULL;
    g_scheduler.active_fibers = 0;
    g_scheduler.is_running = 0;

    g_scheduler.main_fiber = (fiber_t *)fiber_malloc(sizeof(fiber_t));
    if (!g_scheduler.main_fiber) return -1;
    
    g_scheduler.main_fiber->id = 0;
    g_scheduler.main_fiber->state = FIBER_RUNNING;
    g_scheduler.main_fiber->stack = NULL;
    g_scheduler.main_fiber->stack_size = 0;
    g_scheduler.main_fiber->next = NULL;
    g_scheduler.main_fiber->prev = NULL;

    return 0;
}

void scheduler_shutdown(void) {
    fiber_t *curr = g_scheduler.ready_head;
    while (curr) {
        fiber_t *next = curr->next;
        fiber_destroy(curr);
        curr = next;
    }
    g_scheduler.ready_head = NULL;
    g_scheduler.ready_tail = NULL;

    if (g_scheduler.main_fiber) {
        fiber_free(g_scheduler.main_fiber);
        g_scheduler.main_fiber = NULL;
    }
    g_scheduler.current_fiber = NULL;
    g_scheduler.active_fibers = 0;
    g_scheduler.is_running = 0;
}

int scheduler_spawn(fiber_func_t func, void *arg) {
    fiber_t *f = fiber_create(func, arg, 0);
    if (!f) return -1;
    return scheduler_spawn_fiber(f);
}

int scheduler_spawn_fiber(fiber_t *fiber) {
    if (!fiber) return -1;
    fiber->state = FIBER_READY;
    enqueue_ready(fiber);
    g_scheduler.active_fibers++;
    return 0;
}

void scheduler_run(void) {
    if (!g_scheduler.main_fiber) {
        if (scheduler_init() != 0) return;
    }

    g_scheduler.is_running = 1;

    while (g_scheduler.ready_head != NULL) {
        fiber_t *f = dequeue_ready();
        if (!f) break;

        if (f->state == FIBER_TERMINATED) {
            if (g_scheduler.active_fibers > 0) g_scheduler.active_fibers--;
            fiber_destroy(f);
            continue;
        }

        f->state = FIBER_RUNNING;
        g_scheduler.current_fiber = f;

        fiber_context_swap(&g_scheduler.main_fiber->context, &f->context);

        g_scheduler.current_fiber = NULL;

        if (f->state == FIBER_READY) {
            enqueue_ready(f);
        } else if (f->state == FIBER_TERMINATED) {
            if (g_scheduler.active_fibers > 0) g_scheduler.active_fibers--;
            fiber_destroy(f);
        } else if (f->state == FIBER_BLOCKED) {
            // Fiber remains blocked off the ready queue until unblocked.
        }
    }

    g_scheduler.is_running = 0;
}

void scheduler_yield(void) {
    fiber_t *curr = g_scheduler.current_fiber;
    if (!curr || !g_scheduler.main_fiber) return;

    if (curr->state == FIBER_RUNNING) {
        curr->state = FIBER_READY;
    }

    fiber_context_swap(&curr->context, &g_scheduler.main_fiber->context);
}

void scheduler_block_current(fiber_state_t reason) {
    fiber_t *curr = g_scheduler.current_fiber;
    if (!curr || !g_scheduler.main_fiber) return;

    curr->state = (reason == FIBER_BLOCKED) ? FIBER_BLOCKED : reason;
    fiber_context_swap(&curr->context, &g_scheduler.main_fiber->context);
}

void scheduler_unblock(fiber_t *fiber) {
    if (!fiber) return;
    fiber->state = FIBER_READY;
    enqueue_ready(fiber);
}

fiber_t *scheduler_get_current_fiber(void) {
    return g_scheduler.current_fiber;
}

size_t scheduler_get_active_count(void) {
    return g_scheduler.active_fibers;
}

scheduler_t *scheduler_get_instance(void) {
    return &g_scheduler;
}
