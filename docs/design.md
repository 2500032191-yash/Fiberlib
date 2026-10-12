# FiberLib Technical Design Specification

## Design Rationale & Low-Level Mechanics

### 1. POSIX `makecontext` 64-Bit Pointer Packing

The standard POSIX `makecontext` specification requires argument parameters to be passed as standard C integers (`int`). On 64-bit architectures (`x86_64`, `aarch64`), standard pointers are 64 bits (`uintptr_t`), while `int` is 32 bits.

Passing a 64-bit pointer directly as a single argument leads to pointer truncation and memory faults. **FiberLib** solves this using bitwise pointer splitting and reconstruction:

```c
// Splitting in context.c:
uintptr_t val = (uintptr_t)fiber_ptr;
uint32_t hi = (uint32_t)(val >> 32);
uint32_t lo = (uint32_t)(val & 0xFFFFFFFF);
makecontext(ctx, entry_trampoline, 2, hi, lo);

// Reconstructing in fiber.c trampoline:
uintptr_t ptr_val = ((uintptr_t)hi << 32) | (uintptr_t)lo;
fiber_t *f = (fiber_t *)ptr_val;
```

---

### 2. Page-Aligned Stack Allocation via `mmap`

Standard heap allocation using `malloc` for execution stacks can suffer from fragmentation and lack page alignment. FiberLib uses direct kernel memory mapping (`mmap` with `MAP_PRIVATE | MAP_ANONYMOUS`).

Key advantages:
- Guarantee of system page alignment (`sysconf(_SC_PAGESIZE)`).
- Isolation from standard heap allocations.
- Immediate release of virtual memory back to the kernel via `munmap`.

---

### 3. Cooperative Mutex & Semaphore Blocking Queue

In traditional kernel threads, blocking on a mutex puts the OS thread to sleep via futexes/syscalls. In FiberLib green threads:

1. When a fiber calls `fiber_mutex_lock()` and the mutex is locked, the fiber adds itself to `mutex->wait_queue`.
2. The fiber calls `scheduler_block_current(FIBER_BLOCKED)`, which context-switches execution back to `scheduler_run()` without placing the fiber back on the ready queue.
3. The CPU remains active running other ready fibers.
4. When `fiber_mutex_unlock()` is invoked, the owner dequeues the waiting fiber from `mutex->wait_queue` and calls `scheduler_unblock(waiting_fiber)`, placing it back on the ready queue.

---

### 4. Deadlock Detection Diagnostic Log

`fiber_sync_check_deadlock()` inspects the scheduler state. If `active_fibers > 0` but `ready_head == NULL` and `current_fiber == NULL`, all active fibers are blocked in wait queues with no runnable fibers available to make progress. The detector logs a diagnostic warning message indicating a deadlock.
