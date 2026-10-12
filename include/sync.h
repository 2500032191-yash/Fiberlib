#ifndef SYNC_H
#define SYNC_H

#include "fiber.h"

typedef struct {
    int locked;
    fiber_t *owner;
    fiber_t *wait_queue_head;
    fiber_t *wait_queue_tail;
} fiber_mutex_t;

typedef struct {
    int value;
    fiber_t *wait_queue_head;
    fiber_t *wait_queue_tail;
} fiber_sem_t;

void fiber_mutex_init(fiber_mutex_t *mutex);
void fiber_mutex_lock(fiber_mutex_t *mutex);
int fiber_mutex_trylock(fiber_mutex_t *mutex);
void fiber_mutex_unlock(fiber_mutex_t *mutex);
void fiber_mutex_destroy(fiber_mutex_t *mutex);

void fiber_sem_init(fiber_sem_t *sem, int initial_value);
void fiber_sem_wait(fiber_sem_t *sem);
void fiber_sem_post(fiber_sem_t *sem);
void fiber_sem_destroy(fiber_sem_t *sem);

int fiber_sync_check_deadlock(void);

#endif /* SYNC_H */
