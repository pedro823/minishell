#include "process_queue.h"
#include "_aux.h"
#include "error_handler.h"

void proc_queue_add(deque *proc_queue, struct process element) {
    add_to_stack("process_queue->add");
    if (proc_queue == NULL) {
        proc_queue = emalloc(sizeof(deque));
        proc_queue->head = NULL;
        proc_queue->tail = NULL;
    }
    // does it have no elements?
    if (proc_queue->head == NULL) {
        if (proc_queue->tail != NULL) {
            die_with_msg("head of queue is NULL but tail is %p", proc_queue->tail);
        }
        proc_queue->head = emalloc(sizeof(struct process));
        proc_queue->tail = proc_queue->head;
        *(proc_queue->tail) = { 0, 0, 0, NULL, NULL, NULL };
    }

    pop_stack();
}
