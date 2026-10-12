# FiberLib Architectural Documentation

## Executive Overview

**FiberLib** provides cooperative user-space green threads (fibers) for POSIX compliant operating systems. Unlike kernel threads managed by OS kernel schedulers, green threads run inside a single OS thread and switch execution contexts entirely in user space.

---

## Component Architecture

FiberLib is organized into five decoupled subsystem layers:

1. **Fiber Abstraction Layer (`fiber.h`, `fiber.c`)**: Manages individual fiber metadata, stack buffers, control state, entry trampolines, and lifecycle operations.
2. **Scheduling Subsystem (`scheduler.h`, `scheduler.c`)**: Implements a Round-Robin run-queue. Maintains active fiber state, context switching loop, and yield mechanisms.
3. **Context Layer (`context.h`, `context.c`)**: Wraps POSIX `ucontext_t` APIs (`getcontext`, `makecontext`, `swapcontext`). Provides 64-bit platform argument packing for `makecontext`.
4. **Memory Subsystem (`memory.h`, `memory.c`)**: Manages stack allocation using system page-aligned `mmap` (`MAP_ANONYMOUS`) and maintains memory allocation statistics.
5. **Synchronization Subsystem (`sync.h`, `sync.c`)**: Provides cooperative mutexes (`fiber_mutex_t`), semaphores (`fiber_sem_t`), wait queues, and basic deadlock detection diagnostics.

---

## Execution & Lifecycle Model

### State Machine

```
              +-------------------+
              |   FIBER_CREATED   |
              +---------+---------+
                        | (scheduler_spawn)
                        v
              +-------------------+
              |    FIBER_READY    | <----+ (unblocked / yield)
              +---------+---------+      |
                        | (scheduled)    |
                        v                |
              +-------------------+      |
  +---------> |   FIBER_RUNNING   | -----+
  |           +----+---------+----+ (yield)
  |                |         |
  | (unblock)      | (lock)  | (finish / exit)
  |                v         v
  |           +----+----+  +----+---------+
  +---------- | BLOCKED |  | TERMINATED   |
              +---------+  +--------------+
```

### Context Switch Flow

1. **Scheduler Loop**:
   - `scheduler_run()` dequeues the head fiber `F` from `ready_head`.
   - Transitions `F->state = FIBER_RUNNING`.
   - Calls `swapcontext(&main_fiber->context, &F->context)`.

2. **Fiber Execution**:
   - Execution resumes in `F`'s call stack.
   - When `F` calls `fiber_yield()`, `F->state` is set to `FIBER_READY`, and `swapcontext(&F->context, &main_fiber->context)` returns execution to `scheduler_run()`.
   - `scheduler_run()` re-enqueues `F` at the tail of `ready_tail`.

3. **Termination**:
   - When entry function completes or `fiber_exit()` is invoked, `F->state = FIBER_TERMINATED`.
   - Control swaps back to `scheduler_run()`, which cleans up `F` and releases its stack buffer via `fiber_destroy()`.
