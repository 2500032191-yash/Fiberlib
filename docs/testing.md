# FiberLib Testing Strategy & Documentation

## Overview

FiberLib includes a dedicated unit test suite in `tests/` designed to validate subsystem stability, memory cleanliness, context switching correctness, and synchronization primitives.

---

## Test Suites

1. **`test_fiber.c`**:
   - `test_fiber_creation`: Verifies fiber struct initialization, ID assignment, state setting (`FIBER_CREATED`), and stack pointer non-null check.
   - `test_fiber_state_string`: Validates correct string representation for all states (`CREATED`, `READY`, `RUNNING`, `BLOCKED`, `TERMINATED`).

2. **`test_scheduler.c`**:
   - `test_scheduler_lifecycle`: Validates init/shutdown behavior and active fiber counter tracking.
   - `test_scheduler_execution_order`: Verifies Round-Robin interleaved execution order across multiple yielding fibers.

3. **`test_memory.c`**:
   - `test_stack_alloc_free`: Validates `mmap` stack allocation and `munmap` stack release updating global memory tracking statistics.
   - `test_heap_alloc_free`: Validates tracked heap allocation (`fiber_malloc`) and free (`fiber_free`).

4. **`test_sync.c`**:
   - `test_mutex_basic`: Validates `fiber_mutex_init`, `fiber_mutex_trylock`, `fiber_mutex_unlock`, and `fiber_mutex_destroy`.
   - `test_mutex_concurrency`: Validates mutual exclusion across concurrent fibers incrementing a shared resource.
   - `test_sem_basic`: Validates semaphore token counter decrement on `wait` and increment on `post`.

---

## Execution Commands

### Run Unit Test Suite
```bash
make test
```

Expected Output Format:
```
=========================================
 RUNNING FIBERLIB UNIT TESTS
=========================================
Running bin/test_fiber...
[PASS] fiber_create allocates fiber struct
[PASS] fiber_create assigns positive fiber ID
...
ALL UNIT TESTS PASSED SUCCESSFULLY!
```

### Run Demonstration Binaries
```bash
make run-examples
```
