#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "fiber.h"
#include "scheduler.h"
#include "context.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>

static fiber_id_t g_next_id = 1;

static void fiber_trampoline(uint32_t hi, uint32_t lo) {
    uintptr_t ptr_val = ((uintptr_t)hi << 32) | (uintptr_t)lo;
    fiber_t *f = (fiber_t *)ptr_val;

    if (f && f->entry_func) {
        f->entry_func(f->arg);
    }
    
    if (f) {
        f->state = FIBER_TERMINATED;
    }
    fiber_exit();
}

fiber_t *fiber_create(fiber_func_t func, void *arg, size_t stack_size) {
    if (!func) return NULL;
    if (stack_size == 0) stack_size = FIBER_DEFAULT_STACK_SIZE;

    fiber_t *f = (fiber_t *)fiber_malloc(sizeof(fiber_t));
    if (!f) return NULL;

    f->id = g_next_id++;
    f->state = FIBER_CREATED;
    f->entry_func = func;
    f->arg = arg;
    f->stack_size = stack_size;
    f->next = NULL;
    f->prev = NULL;

    f->stack = fiber_stack_alloc(stack_size);
    if (!f->stack) {
        fiber_free(f);
        return NULL;
    }

    if (fiber_context_init(&f->context, f->stack, f->stack_size, NULL, NULL) != 0) {
        fiber_stack_free(f->stack, f->stack_size);
        fiber_free(f);
        return NULL;
    }

    fiber_context_make_arg(&f->context, (void (*)(void))fiber_trampoline, f);
    return f;
}

void fiber_destroy(fiber_t *fiber) {
    if (!fiber) return;
    if (fiber->stack) {
        fiber_stack_free(fiber->stack, fiber->stack_size);
        fiber->stack = NULL;
    }
    fiber_free(fiber);
}

void fiber_yield(void) {
    scheduler_yield();
}

void fiber_exit(void) {
    fiber_t *curr = scheduler_get_current_fiber();
    if (curr) {
        curr->state = FIBER_TERMINATED;
    }
    scheduler_yield();
}

fiber_t *fiber_current(void) {
    return scheduler_get_current_fiber();
}

fiber_id_t fiber_get_id(const fiber_t *fiber) {
    return fiber ? fiber->id : 0;
}

const char *fiber_state_to_string(fiber_state_t state) {
    switch (state) {
        case FIBER_CREATED: return "CREATED";
        case FIBER_READY: return "READY";
        case FIBER_RUNNING: return "RUNNING";
        case FIBER_BLOCKED: return "BLOCKED";
        case FIBER_TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}
