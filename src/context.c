#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "context.h"
#include <stdio.h>
#include <stdlib.h>

int fiber_context_get(ucontext_t *ctx) {
    if (!ctx) return -1;
    return getcontext(ctx);
}

int fiber_context_init(ucontext_t *ctx, void *stack, size_t stack_size, void (*entry_point)(void), ucontext_t *link_ctx) {
    if (!ctx || !stack || stack_size == 0) return -1;

    if (getcontext(ctx) == -1) {
        perror("fiber_context_init: getcontext failed");
        return -1;
    }

    ctx->uc_stack.ss_sp = stack;
    ctx->uc_stack.ss_size = stack_size;
    ctx->uc_stack.ss_flags = 0;
    ctx->uc_link = link_ctx;

    if (entry_point) {
        makecontext(ctx, entry_point, 0);
    }

    return 0;
}

int fiber_context_swap(ucontext_t *old_ctx, const ucontext_t *new_ctx) {
    if (!old_ctx || !new_ctx) return -1;
    return swapcontext(old_ctx, (ucontext_t *)new_ctx);
}

void fiber_context_make_arg(ucontext_t *ctx, void (*entry)(void), void *arg) {
    uintptr_t val = (uintptr_t)arg;
    uint32_t hi = (uint32_t)(val >> 32);
    uint32_t lo = (uint32_t)(val & 0xFFFFFFFF);
    makecontext(ctx, entry, 2, hi, lo);
}
