# FiberLib - User-Level Green Thread Library in C

**FiberLib** is a high-performance, lightweight user-level green thread library implemented in C for POSIX systems (Linux & WSL). It leverages POSIX context manipulation primitives (`getcontext`, `makecontext`, `swapcontext`) to manage cooperative fiber execution, scheduling, and synchronization without kernel thread overhead.

---

## Key Features

- **Cooperative Green Thread Management**: Creation, state transitions (`CREATED`, `READY`, `RUNNING`, `BLOCKED`, `TERMINATED`), yielding, and termination.
- **Round-Robin Scheduler**: FIFO ready queue for smooth cooperatively scheduled fiber execution.
- **Stack & Memory Management**: Page-aligned dynamic stack allocation via `mmap` (`MAP_ANONYMOUS`) with allocation & cleanup tracking.
- **Synchronization Primitives**: Mutexes (`fiber_mutex_t`), Semaphores (`fiber_sem_t`), cooperative wait queues, and basic deadlock detection logging.
- **POSIX System Integration Demos**: Process management (`fork`), Anonymous Pipes (`pipe`), Signal Handling (`sigaction`), Low-level File I/O (`open`/`write`/`read`/`close`), and Memory layout visualization.
- **Zero Warnings**: Compiles cleanly with `gcc -Wall -Wextra -Iinclude`.

---

## Directory Structure

```
FiberLib/
├── include/
│   ├── fiber.h        # Fiber state, handle, and life-cycle API
│   ├── scheduler.h    # Round-Robin scheduler structure and run-queue API
│   ├── context.h      # POSIX context management wrappers
│   ├── memory.h       # Stack allocation (mmap) and memory tracking helpers
│   └── sync.h         # Mutex, Semaphore, and Deadlock detection API
├── src/
│   ├── fiber.c        # Fiber creation, allocation, yield, exit logic
│   ├── scheduler.c    # Round-Robin scheduling loop and queue rotation
│   ├── context.c      # POSIX ucontext wrappers and 64-bit argument packing
│   ├── memory.c       # Page-aligned mmap stack allocator & tracker
│   └── sync.c         # Mutex, semaphore, and deadlock detection
├── examples/          # 8 structured example demos with diagnostic logging
│   ├── basic_fibers.c
│   ├── scheduling.c
│   ├── process_demo.c
│   ├── ipc_demo.c
│   ├── signal_demo.c
│   ├── memory_demo.c
│   ├── file_io_demo.c
│   └── synchronization_demo.c
├── tests/             # Comprehensive unit testing suite
│   ├── test_fiber.c
│   ├── test_scheduler.c
│   ├── test_memory.c
│   └── test_sync.c
├── docs/              # In-depth architectural & design documentation
│   ├── architecture.md
│   ├── design.md
│   └── testing.md
├── Makefile           # Static library compilation & test target
└── README.md
```

---

## Building and Running

### Build Static Library and Binaries

```bash
make all
```

This generates `lib/libfiber.a` and all executable binaries in `bin/`.

### Run Unit Tests

```bash
make test
```

Executes `bin/test_fiber`, `bin/test_scheduler`, `bin/test_memory`, and `bin/test_sync`. All tests output formatted `[PASS]` / `[FAIL]` status indicators.

### Run All 8 Demos

```bash
make run-examples
```

---

## API Quickstart

```c
#include "fiber.h"
#include "scheduler.h"
#include "sync.h"
#include <stdio.h>

static fiber_mutex_t g_mutex;

static void worker(void *arg) {
    (void)arg;
    fiber_mutex_lock(&g_mutex);
    printf("Fiber %lu executing inside critical section...\n", fiber_get_id(fiber_current()));
    fiber_yield();
    fiber_mutex_unlock(&g_mutex);
}

int main(void) {
    scheduler_init();
    fiber_mutex_init(&g_mutex);

    scheduler_spawn(worker, NULL);
    scheduler_spawn(worker, NULL);

    scheduler_run();

    fiber_mutex_destroy(&g_mutex);
    scheduler_shutdown();
    return 0;
}
```
