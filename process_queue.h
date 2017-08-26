/*
    Process queue for Scheduler
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#ifndef __PROC_QUEUE_H__
#define __PROC_QUEUE_H__

#include "_aux.h"


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

// Adds new process to queue on TAIL
void proc_queue_add(deque *proc_queue, struct process element);

// Reverse adds (adds on HEAD)
void proc_queue_radd(deque *proc_queue, struct process element);

// Prints process queue (HEAD to TAIL)
void print_proc_queue(deque *proc_queue);

// Removes process from the head of the queue
struct process pop_head(deque *proc_queue);

// Removes process from the tail of the queue
struct process pop_tail(deque *proc_queue);

// Is the process queue empty?
bool is_empty(deque *proc_queue);

#endif
