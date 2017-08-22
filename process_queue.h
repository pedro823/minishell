/*
    Process queue for Scheduler
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */

 // Double-ended linked list queue
 struct process {
     float init_time;
     float dt;
     float deadline;
     char *name;
     struct process *next;
     struct process *prev;
 };

 typedef struct process *node;

typedef struct {
    node head;
    node tail;
} deque;

// Adds new process to queue
void proc_queue_add(deque *proc_queue, struct process element);

// Reverse adds (adds to head)
void proc_queue_radd(deque *proc_queue, struct process element);

// Removes process from the head of the queue
struct process pop_head(deque *proc_queue);

// Removes process from the tail of the queue
struct process pop_tail(deque *proc_queue);
