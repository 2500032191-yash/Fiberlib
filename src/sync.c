#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "sync.h"
#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

static void enqueue_wait(fiber_t **head, fiber_t **tail, fiber_t *f) {
    if (!f) return;
    f->next = NULL;
    f->prev = *tail;
    if (*tail) {
        (*tail)->next = f;
    } else {
        *head = f;
    }
    *tail = f;
}

static fiber_t *dequeue_wait(fiber_t **head, fiber_t **tail) {
    if (!*head) return NULL;
    fiber_t *f = *head;
    *head = f->next;
    if (*head) {
        (*head)->prev = NULL;
    } else {
        *tail = NULL;
    }
    f->next = NULL;
    f->prev = NULL;
    return f;
}

void fiber_mutex_init(fiber_mutex_t *mutex) {
    if (!mutex) return;
    mutex->locked = 0;
    mutex->owner = NULL;
    mutex->wait_queue_head = NULL;
    mutex->wait_queue_tail = NULL;
}

void fiber_mutex_lock(fiber_mutex_t *mutex) {
    if (!mutex) return;
    fiber_t *curr = fiber_current();

    while (mutex->locked) {
        if (curr) {
            enqueue_wait(&mutex->wait_queue_head, &mutex->wait_queue_tail, curr);
            scheduler_block_current(FIBER_BLOCKED);
        } else {
            break;
        }
    }

    mutex->locked = 1;
    mutex->owner = curr;
}

int fiber_mutex_trylock(fiber_mutex_t *mutex) {
    if (!mutex) return -1;
    if (!mutex->locked) {
        mutex->locked = 1;
        mutex->owner = fiber_current();
        return 0;
    }
    return -1;
}

void fiber_mutex_unlock(fiber_mutex_t *mutex) {
    if (!mutex || !mutex->locked) return;
    
    mutex->locked = 0;
    mutex->owner = NULL;

    if (mutex->wait_queue_head) {
        fiber_t *next_fiber = dequeue_wait(&mutex->wait_queue_head, &mutex->wait_queue_tail);
        if (next_fiber) {
            scheduler_unblock(next_fiber);
        }
    }
}

void fiber_mutex_destroy(fiber_mutex_t *mutex) {
    if (!mutex) return;
    mutex->locked = 0;
    mutex->owner = NULL;
    mutex->wait_queue_head = NULL;
    mutex->wait_queue_tail = NULL;
}

void fiber_sem_init(fiber_sem_t *sem, int initial_value) {
    if (!sem) return;
    sem->value = initial_value;
    sem->wait_queue_head = NULL;
    sem->wait_queue_tail = NULL;
}

void fiber_sem_wait(fiber_sem_t *sem) {
    if (!sem) return;
    fiber_t *curr = fiber_current();

    sem->value--;
    if (sem->value < 0) {
        if (curr) {
            enqueue_wait(&sem->wait_queue_head, &sem->wait_queue_tail, curr);
            scheduler_block_current(FIBER_BLOCKED);
        }
    }
}

void fiber_sem_post(fiber_sem_t *sem) {
    if (!sem) return;

    sem->value++;
    if (sem->value <= 0 && sem->wait_queue_head) {
        fiber_t *next_fiber = dequeue_wait(&sem->wait_queue_head, &sem->wait_queue_tail);
        if (next_fiber) {
            scheduler_unblock(next_fiber);
        }
    }
}

void fiber_sem_destroy(fiber_sem_t *sem) {
    if (!sem) return;
    sem->value = 0;
    sem->wait_queue_head = NULL;
    sem->wait_queue_tail = NULL;
}

int fiber_sync_check_deadlock(void) {
    scheduler_t *sched = scheduler_get_instance();
    if (!sched) return 0;

    if (sched->active_fibers > 0 && sched->ready_head == NULL && sched->current_fiber == NULL) {
        printf("[DEADLOCK DETECTOR] WARNING: Deadlock detected! %zu fibers active but no ready fibers in queue.\n", sched->active_fibers);
        return 1;
    }
    return 0;
}
