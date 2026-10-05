*This project has been created as part of the 42 curriculum by esnavarr.*

# Codexion

> Master the race for resources before the deadline masters you.

## Description

**Codexion** is a concurrency simulation written in C with POSIX threads.
It is a variation of the classic
[dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem):
the philosophers are **coders**, the forks are **USB dongles**,
and eating is **compiling quantum code**.

Coders sit in a circle, with one dongle between each pair of neighbours.
Each coder repeats the same cycle:

1. **Compile**: needs two dongles at the same time (left and right).
2. **Debug**: the dongles are put back on the table.
3. **Refactor**: then the coder immediately asks for the dongles again.

The rules that make it hard:

- A dongle can only be held by one coder at a time.
- After being released, a dongle stays unavailable for `dongle_cooldown` ms.
- A coder who does not *start* compiling within `time_to_burnout` ms
  (counted from the start of their last compile, or from the start of the simulation)
  **burns out**, and the simulation stops.
- When several coders want the same dongle, a **scheduler** decides who gets it:
  - **`fifo`** (First In, First Out): the request that arrived first is served first.
  - **`edf`** (Earliest Deadline First): the coder whose burnout deadline
    (`last_compile_start + time_to_burnout`) is the closest is served first.
    Equal deadlines are broken by arrival order.

The simulation ends when every coder has compiled `number_of_compiles_required` times,
or as soon as one coder burns out.

The goal of the project is to coordinate the threads so that there are
no deadlocks, no data races, no starvation, precise timing and clean logs.

## Instructions

### Compilation

```bash
make        # builds ./codexion
make clean  # removes object files
make fclean # removes object files and the binary
make re     # rebuilds everything
make norm   # runs norminette on the sources
```

The project compiles with `cc -Wall -Wextra -Werror -pthread` and has no dependencies.

### Usage

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning | Valid values |
| --- | --- | --- |
| `number_of_coders` | Number of coders, and of dongles | 1 to 200 |
| `time_to_burnout` | Max time between two compile starts (ms) | ≥ 1 |
| `time_to_compile` | Duration of a compilation (ms) | ≥ 1 |
| `time_to_debug` | Duration of debugging (ms) | ≥ 1 |
| `time_to_refactor` | Duration of refactoring (ms) | ≥ 1 |
| `number_of_compiles_required` | Compilations each coder must do | ≥ 1 |
| `dongle_cooldown` | Time a released dongle stays unavailable (ms) | ≥ 0 |
| `scheduler` | Arbitration policy | `fifo` or `edf` |

Any other input (missing argument, negative number, non-digit characters,
number that does not fit in an `int`, unknown scheduler) is rejected
with an error message and exit status `1`.

### Output

Every change of state is printed on its own line, as
`timestamp_in_ms coder_number action`:

```text
$ ./codexion 5 2000 200 200 200 10 0 fifo
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
0 3 has taken a dongle
0 3 has taken a dongle
0 3 is compiling
200 1 is debugging
200 3 is debugging
200 5 has taken a dongle
200 5 has taken a dongle
200 5 is compiling
...
```

If a coder burns out, `timestamp X burned out` is the last line printed.

### Examples

```bash
./codexion 1 800 200 200 200 10 0 fifo      # one dongle only: coder 1 burns out at ~800
./codexion 5 2000 200 200 200 10 0 fifo     # no burnout, stops after 10 compiles each
./codexion 5 2000 200 200 200 7 0 edf       # same with the edf scheduler
./codexion 5 500 200 200 200 10 0 fifo      # impossible (cycle of 600 > 500): burnout at ~500
./codexion 5 3000 200 200 200 10 800 fifo   # heavy contention because of the cooldown
```

### Testing for data races, deadlocks and leaks

```bash
valgrind --leak-check=full ./codexion 5 2000 200 200 200 3 0 fifo
valgrind --tool=helgrind ./codexion 5 2000 200 200 200 3 0 fifo
valgrind --tool=drd ./codexion 5 2000 200 200 200 3 0 edf
cc -Iinclude -pthread -fsanitize=thread -g src/*.c -o codexion_tsan
```

### Project structure

| File | Content |
| --- | --- |
| `main.c` | Creates the context, dongles and coders, starts and joins the threads |
| `context.c` | Parses and validates the arguments, holds the shared state |
| `get.c` | Converts arguments: numbers and scheduler name |
| `setup.c` | Creates and destroys the arrays of dongles and coders |
| `coder.c` | Creates and destroys a coder |
| `coder_routine.c` | Life of a coder: take dongles, compile, debug, refactor |
| `coder_routine_utils.c` | Takes both dongles through the scheduler queues |
| `dongle.c` | Creates, checks, waits for and releases a dongle |
| `cmp.c` | The schedulers: `cmp_fifo` and `cmp_edf` |
| `pqueue.c`, `pqueue_ops.c` | Priority queue implemented as a binary heap |
| `monitor.c` | Monitor thread: detects burnouts and the end of the simulation |
| `log_state.c` | Serialized logging, stops the simulation |
| `time_utils.c` | Current time and precise sleeping |
| `traceback.c` | Error messages |

## Blocking cases handled

### Deadlock: Coffman's conditions

A deadlock can only happen when four conditions are true at the same time
(Coffman, 1971). Here is how each one is handled:

| Condition | In Codexion |
| --- | --- |
| Mutual exclusion | Required by the subject: a dongle has a single owner. |
| **Hold and wait** | **Broken.** A coder takes both of its dongles in one step, or none. It never holds one dongle while waiting for the other. |
| No preemption | Kept: a dongle is only released after compiling. |
| **Circular wait** | **Broken.** The mutexes of the two dongles of a coder are always locked in the same global order: lowest dongle index first. |

![Diagram of a circular wait scenario](https://diningphilosophers.eu/pictures/deadlock.png)
> Image from [diningphilosophers.eu](https://diningphilosophers.eu/)

### Starvation and fair arbitration

Each dongle has a **waiting queue implemented as a binary heap** (`pqueue`).
The heap is ordered by the scheduler's comparator, so the request that must be served next
is always at the root:

- `cmp_fifo` compares **tickets**: the smallest ticket (oldest request) wins.
- `cmp_edf` compares **deadlines**, then tickets if the deadlines are equal,
  so the policy is fully deterministic.

When a coder wants to compile, it creates **one request** with
its number, its burnout deadline and a **ticket**.
Tickets come from a single counter, in the order the requests arrive.
The request is pushed into the queue of **both** of its dongles.
The coder may only take its dongles when:

1. its request is **first in both queues**,
2. **both dongles are free**,
3. **both cooldowns are over**.

Because all queues use the same total order,
the request that must be served first overall is always first in both of its queues.
It can therefore never be blocked by another request: the simulation always makes progress.
With `edf`, the coder closest to burning out is served first,
which guarantees liveness whenever the parameters are feasible.

At the start, **even-numbered coders wait for 1 ms**,
so the odd-numbered coders get their dongles first and the others queue behind them.

### Cooldown handling

When a dongle is released, it records the exact time at which its cooldown ends
(`available_at`). A dongle is only given to a coder once `available_at` has passed,
so it can never be taken during its cooldown.
A coder who is first in the queue and only waiting for the cooldown
releases the dongle's mutex and sleeps in short slices (500 µs at most)
until `available_at`, then checks again.

### Precise burnout detection

- A **monitor thread** checks every coder **every millisecond**.
- A coder burns out when **more than `time_to_burnout` milliseconds** have passed
  since the start of its last compile. The check works in whole milliseconds,
  like the arguments and the logs.
- The burnout is printed 1 to 2 ms after the deadline, well within the 10 ms limit.
- All coders are checked for burnout first; only then does the monitor check
  whether every coder has compiled enough times.

Coders sleep in short slices of 500 µs towards a fixed end time.
Durations stay precise (no drift), and a coder notices within half a millisecond
that the simulation has stopped.

### Log serialization

- Every log line is printed while holding the **print mutex**,
  so two lines can never be mixed.
- The stop flag is checked **inside** that critical section.
  The monitor prints the burnout line and sets the stop flag in the same critical section,
  so **nothing can be printed after "burned out"**.
- A coder prints "is debugging" **before** releasing its dongles.
  A neighbour can therefore never print "has taken a dongle"
  for a dongle that the log still shows in use.

### Other edge cases

- **A single coder** has only one dongle: it takes it, can never compile,
  and burns out at `time_to_burnout`.
- **Thread creation failure**: the simulation is stopped,
  the threads already created are woken up and joined, and all memory is freed.
- **End of the simulation**: the monitor broadcasts on every dongle's condition variable,
  so no coder stays asleep waiting for a dongle.

## Thread synchronization mechanisms

### Primitives

| Primitive | Protects | Used by |
| --- | --- | --- |
| `pthread_mutex_t` per dongle | `in_use`, `available_at`, queue of requests | Coders, monitor |
| `pthread_cond_t` per dongle | Wakes the coders waiting for this dongle | Coders, monitor |
| `pthread_mutex_t` per coder (`stats_mutex`) | `last_compile`, `compiles` | The coder (writes), monitor (reads) |
| `ticket_mutex` | Counter that numbers the requests | Coders |
| `stop_mutex` | `stop` flag | Everyone |
| `print_mutex` | `stdout` | Everyone |

### How a coder takes its dongles

```text
lock first dongle, lock second dongle
take a ticket, push the same request into both queues
while not (ready on first dongle and ready on second dongle):
    unlock the dongle that is ready, wait on the condition variable of the one that is not
    (if only its cooldown is missing: unlock it and sleep until the cooldown ends)
    lock both dongles again (in order)
pop the request from both queues, mark both dongles in use
unlock both dongles
```

While sleeping, a coder holds only the mutex of the dongle that blocks it,
and `pthread_cond_wait` releases it. Releasing a dongle broadcasts on its condition variable,
so the waiting coders wake up and check again whether it is their turn.

### Examples of prevented race conditions

- **Two coders taking the same dongle**: `in_use` is only read and written
  while holding the dongle's mutex, and a coder must be first in the queue to take it.
- **Monitor reading a half-updated coder**: the coder writes `last_compile` and `compiles`
  while holding its `stats_mutex`. The monitor reads them while holding the same mutex.
- **A log printed after the burnout**: see *Log serialization* above.
- **A coder sleeping forever after the end**: the stop flag is set first,
  then the monitor locks each dongle and broadcasts.
  A coder always checks the flag while holding the dongle's mutex, right before waiting,
  so the wake-up cannot be missed.

### Communication between coders and the monitor

Coders and the monitor never call each other: they only share protected variables.
Coders publish their progress (`last_compile`, `compiles`); the monitor publishes
the decision to stop (`stop`). There are no global variables: everything is reachable
from the context passed to each thread.

### Lock order

Locks are always taken in the same order, which prevents lock-ordering deadlocks:

- first dongle → second dongle → `ticket_mutex` or `stop_mutex`
- `print_mutex` → `stop_mutex`

## Resources

### References
- [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem): the problem Codexion is based on.
- [diningphilosophers.eu](https://diningphilosophers.eu/): a website entirely dedicated to the problem.
- E. G. Coffman, M. Elphick, A. Shoshani, *System Deadlocks* (1971): the four conditions for a deadlock.
- [Earliest deadline first scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling).
- [Binary heap](https://en.wikipedia.org/wiki/Binary_heap): the structure behind the priority queue.
- [POSIX threads programming (CMU)](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html), including its examples.
- [Multithreading in C (GeeksforGeeks)](https://www.geeksforgeeks.org/c/multithreading-in-c/).
- The manual pages of `pthread_mutex_lock`, `pthread_cond_wait`
  and `pthread_cond_timedwait`.
- A [Codexion visualizer](https://codexion-visualizer.sacha-dev.me/) made by another 42 student,
  very helpful to see what happens in a simulation.

I also learned a lot from my fellow peers, and from students of older cohorts
who did the similar *Philosophers* project.

### Use of AI
AI was used as a learning and reviewing tool:
- to learn multithreading concepts (mutexes, condition variables, deadlock conditions);
- to review the code and find concurrency bugs,
  for example in the monitor and in the order of the logs at the end of the simulation;
- to discuss the design of the dongle acquisition (taking both dongles at once)
  and of the binary heap;
- to write small scripts that check the logs (cooldowns, dongle duplication, burnout timing);
- to restructure and proofread this README.
