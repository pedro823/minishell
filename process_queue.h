/*
    Process queue for Scheduler
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#ifndef __PROC_QUEUE_H__
#define __PROC_QUEUE_H__

#include "_aux.h"
#include <pthread.h>
#include <semaphore.h>

 // Double-ended linked list queue
 struct process {
    float init_time;
    float dt;
    float deadline;
    float time_started;
    float time_ended;
    char *name;
    sem_t *entry_flag;
    sem_t *exit_flag;
    pthread_t thread;
    struct process *next;
    struct process *prev;
};

// struct for calling create_thread
struct sim_arguments {
    struct process *proc; // Process to be simulated
    float quantum;
    int process_number; // for return purposes
};

typedef struct process *node;

typedef struct {
    node head;
    node tail;
} deque;

// Adds new process to queue on TAIL.
// proc_queue is passed through reference
void proc_queue_add(deque **proc_queue, struct process element);

// Reverse adds (adds on HEAD) and returns new LL
// proc_queue is passed through reference
void proc_queue_radd(deque **proc_queue, struct process element);

// Prints process queue (HEAD to TAIL)
void print_proc_queue(deque *proc_queue);

// Prints reverted process queue (TAIL to HEAD)
void print_rev_queue(deque *proc_queue);

// pops from HEAD and inserts on TAIL
void proc_queue_cycle(deque **proc_queue);

// pops from TAIL and inserts on HEAD
void proc_queue_rcycle(deque **proc_queue);

// Removes process from the head of the queue
// proc_queue is passed through reference
struct process pop_head(deque **proc_queue);

// Removes process from the tail of the queue
// proc_queue is passed through reference
struct process pop_tail(deque **proc_queue);

// creates the thread, WITHOUT pthread_join, with function f, on node proc.
void create_thread(node proc, void *(*function) (void *), struct sim_arguments *args);

// Frees a queue
void free_queue(deque **proc_queue);

// Is the process queue empty?
bool is_empty(deque *proc_queue);

#endif
