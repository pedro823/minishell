#include "process_queue.h"
#include "_aux.h"
#include "error_handler.h"
#include <stdio.h>

void proc_queue_add(deque *proc_queue, struct process element) {
    add_to_stack("process_queue->add");
    if (proc_queue == NULL) {
        proc_queue = emalloc(sizeof(deque));
        proc_queue->head = NULL;
        proc_queue->tail = NULL;
    }
    // does it have no elements?
    if (proc_queue->tail == NULL) {
        if (proc_queue->head != NULL) {
            die_with_msg("Head of queue is NULL but tail is %p, with name %s", proc_queue->head, proc_queue->head->name);
        }
        proc_queue->head = emalloc(sizeof(struct process));
        proc_queue->tail = proc_queue->head;

        // Assignments
        *(proc_queue->tail) = element;
        pop_stack();
        return;
    }
    // More than 1 element: appends on TAIL
    node new_process = emalloc(sizeof(struct process));
    *new_process = element;
    new_process->next = NULL;

    new_process->prev = proc_queue->tail;
    proc_queue->tail->next = new_process;

    proc_queue->tail = new_process;

    pop_stack();
}

void proc_queue_radd(deque *proc_queue, struct process element) {
    add_to_stack("process_queue->radd");
    if (proc_queue == NULL) {
        proc_queue = emalloc(sizeof(deque));
        proc_queue->head = NULL;
        proc_queue->tail = NULL;
    }
    // does it have no elements?
    if (proc_queue->head == NULL) {
        if (proc_queue->tail != NULL) {
            die_with_msg("head of queue is NULL but tail is %p, with name %s", proc_queue->tail, proc_queue->tail->name);
        }
        proc_queue->head = emalloc(sizeof(struct process));
        proc_queue->tail = proc_queue->head;
        *(proc_queue->tail) = element;
    }
    // More than 1 element: appends on HEAD
    node new_process = emalloc(sizeof(struct process));
    *new_process = element;
    new_process->prev = NULL;

    new_process->next = proc_queue->head;
    proc_queue->head->prev = new_process;

    proc_queue->head = new_process;

    pop_stack();
}

void print_proc_queue(deque *proc_queue) {
    add_to_stack("process_queue->print");
    printf("Process queue: \n");
    int count = 0;
    for (node i = proc_queue->head; i != NULL; i = i->next) {
        debug_print(0, "proc-count = %d", count);
        printf("%d:\tName: %s\n\tinit_time: %d\n\tdt: %d\n\tdeadline: %d\n", i->name, i->init_time, i->dt, i->deadline);
        count++;
    }
    debug_print(0, "bye!");
    pop_stack();
}

struct process pop_head(deque *proc_queue) {
    add_to_stack("process_queue->pop_head");
    if (proc_queue == NULL) {
        die_with_msg("proc_queue is NULL");
    }
    if (proc_queue->head == NULL) {
        die_with_msg("Tried to pop_head, but head is NULL!");
    }
    if (proc_queue->tail == NULL) {
        die_with_msg("HEAD is %p with name %s, but tail is NULL!", proc_queue->head, proc_queue->head->name);
    }
    struct process popped = *(proc_queue->head);
    node new_head = proc_queue->head->next;
    free(proc_queue->head);
    if (new_head == NULL) {
        proc_queue->tail = NULL;
    }
    else {
        new_head->prev = NULL;
    }
    proc_queue->head = new_head;
    pop_stack();
    return popped;
}

struct process pop_tail(deque *proc_queue) {
    add_to_stack("process_queue->pop_tail");
    if (proc_queue == NULL) {
        die_with_msg("proc_queue is NULL");
    }
    if (proc_queue->tail == NULL) {
        die_with_msg("Tried to pop_tail, but tail is NULL!");
    }
    if (proc_queue->head == NULL) {
        die_with_msg("TAIL is %p with name %s, but head is NULL!", proc_queue->tail, proc_queue->tail->name);
    }
    struct process popped = *(proc_queue->tail);
    node new_tail = proc_queue->tail->prev;
    free(proc_queue->tail);
    if (new_tail == NULL) {
        proc_queue->head = NULL;
    }
    else {
        new_tail->next = NULL;
    }
    proc_queue->tail = new_tail;
    pop_stack();
    return popped;
}

bool is_empty(deque *proc_queue) {
    add_to_stack("process_queue->is_empty");
    if (proc_queue->head == NULL || proc_queue->tail == NULL) {
        if (proc_queue->head != NULL || proc_queue->tail != NULL) {
            die_with_msg("HEAD/TAIL differ: one is NULL, other is not.");
        }
        pop_stack();
        return true;
    }
    pop_stack();
    return false;
}
