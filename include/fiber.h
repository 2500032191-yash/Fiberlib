#ifndef FIBER_H
#define FIBER_H

#include <stddef.h>
#include <stdint.h>
#include <ucontext.h>

typedef uint64_t fiber_id_t;

typedef enum {
    FIBER_CREATED,
    FIBER_READY,
    FIBER_RUNNING,
    FIBER_BLOCKED,
    FIBER_TERMINATED
} fiber_state_t;

typedef void (*fiber_func_t)(void *arg);

typedef struct fiber {
    fiber_id_t id;
    fiber_state_t state;
    void *stack;
    size_t stack_size;
    ucontext_t context;
    fiber_func_t entry_func;
    void *arg;
    struct fiber *next;
    struct fiber *prev;
} fiber_t;

fiber_t *fiber_create(fiber_func_t func, void *arg, size_t stack_size);
void fiber_destroy(fiber_t *fiber);
void fiber_yield(void);
void fiber_exit(void);
fiber_t *fiber_current(void);
fiber_id_t fiber_get_id(const fiber_t *fiber);
const char *fiber_state_to_string(fiber_state_t state);

#endif /* FIBER_H */
