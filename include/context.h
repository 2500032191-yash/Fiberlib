#ifndef CONTEXT_H
#define CONTEXT_H

#include <ucontext.h>
#include <stddef.h>
#include <stdint.h>

int fiber_context_init(ucontext_t *ctx, void *stack, size_t stack_size, void (*entry_point)(void), ucontext_t *link_ctx);
int fiber_context_swap(ucontext_t *old_ctx, const ucontext_t *new_ctx);
int fiber_context_get(ucontext_t *ctx);
void fiber_context_make_arg(ucontext_t *ctx, void (*entry)(void), void *arg);

#endif /* CONTEXT_H */
