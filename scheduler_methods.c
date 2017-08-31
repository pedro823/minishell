/*
    Choices for EP1 part 2
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#include "scheduler_methods.h"
#include "process_queue.h"
#include "_aux.h"
#include "error_handler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// Transforms the linked list into a vector. count is set through reference
// DESTROYS ORIGINAL LIST, SO NO FREE NECESSARY
struct process *to_vector(deque **list, int* size) {
    add_to_stack("scheduler_methods->to_vector");
    int s = 0;
    if (*list == NULL) {
        die_with_msg("No process queue!");
    }
    for (node i = (*list)->head; i != NULL; i = i->next) {
        s++;
    }
    if (s < 1) {
        die_with_msg("no process queue!");
    }
    struct process *vector = emalloc(s * sizeof(struct process));
    node next;
    int c = 0;
    for (node i = (*list)->head; i != NULL; i = next, c++) {
        next = i->next;
        vector[c] = *i;
        free(i);
    }
    *size = c;
    *list = NULL;
    pop_stack();
    return vector;
}

// gets minimum between two floats
float minf(float a, float b) {
    return a < b ? a : b;
}

// Gets priority function of a process
// Higher priority = more important
int get_priority(struct process proc) {
    return proc.dt - proc.deadline; // TODO
}

// simulates wasted time with real CPU usage.
void simulate(float amount, struct process *proc, double *time_passed) {
    add_to_stack("scheduler_methods->simulate");
    float real_amount = minf(amount, proc->dt);
    debug_print(1, "Simulating %s for %.3fs", proc->name, real_amount);
    struct timespec amount_ts = ftots(real_amount);
    if (nanosleep(&amount_ts, NULL) < 0) {
        die_with_msg("nanosleep failed");
    }
    proc->dt -= real_amount;
    // Takes away rounding errors
    if (proc->dt < 1e-5) {
        proc->dt = 0;
    }
    *time_passed += real_amount;
    pop_stack();
}

// Aux merge function
void merge(struct process *job_list, int start, int mid, int end, char mode) {
    add_to_stack("scheduler_methods->merge");
    int i = start, j = mid + 1, k = 0;
    struct process *temp = emalloc((end - start + 1) * sizeof(struct process));
    if (mode == 0) {
        while (i <= mid && j <= end) {
            if (job_list[i].init_time <= job_list[j].init_time) {
                temp[k++] = job_list[i++];
            }
            else {
                temp[k++] = job_list[j++];
            }
        }
        while (i <= mid) {
            temp[k++] = job_list[i++];
        }
        while (j <= end) {
            temp[k++] = job_list[j++];
        }
    }
    else if (mode == 1) {
        while (i <= mid && j <= end) {
            if (job_list[i].dt <= job_list[j].dt) {
                temp[k++] = job_list[i++];
            }
            else {
                temp[k++] = job_list[j++];
            }
        }
        while (i <= mid) {
            temp[k++] = job_list[i++];
        }
        while (j <= end) {
            temp[k++] = job_list[j++];
        }
    }
    else {
        while (i <= mid && j <= end) {
            // Our priority: deadline - dt
            if (get_priority(job_list[i]) <= get_priority(job_list[j])) {
                temp[k++] = job_list[i++];
            }
            else {
                temp[k++] = job_list[j++];
            }
        }
        while (i <= mid) {
            temp[k++] = job_list[i++];
        }
        while (j <= end) {
            temp[k++] = job_list[j++];
        }
    }
    for (i = start; i <= end; i++) {
        job_list[i] = temp[i - start];
    }
    free(temp);
    pop_stack();
}

// Merge-sorts a job list by compare function
// if mode is 0: Sorts by init_time
// if mode is 1: Sorts by dt
// if mode is 2: Sorts by priority
void sort_queue(struct process *job_list, int start, int end, char mode) {
    if (start < end) {
        int mid = (start + end) / 2;
        sort_queue(job_list, start, mid, mode);
        sort_queue(job_list, mid + 1, end, mode);
        merge(job_list, start, mid, end, mode);
    }
}


void round_robin(deque **proc_queue, float quantum) {
    add_to_stack("round_robin");
    int size;
    struct process *order = to_vector(proc_queue, &size);
    sort_queue(order, 0, size - 1, 0);

    for (int i = 0; i < size; i++) {
        debug_print(0, "order[%d].name = %s", i, order[i].name);
    }
    deque *sim_queue = NULL;
    double time_passed = 0;
    // Info for the report
    int amount_completed_in_deadline = 0;
    long long amount_context_change = 0;
    float wait_time = 0;
    float deadline_trespass = 0;

    // Simulates idle until first process arrives
    struct process idle = { 0, 100000000, 100000000, -1, "__IDLE__" };
    proc_queue_add(&sim_queue, idle);
    simulate(order[0].init_time, &idle, &time_passed);
    int proc_index = 0;
    proc_queue_add(&sim_queue, order[proc_index++]);
    // if sim_queue->head == sim_queue->tail, list has only one element: IDLE
    while (proc_index < size || (sim_queue->head != sim_queue->tail)) {
        // Adds processes that reached in this exact quantum
        while(proc_index < size && order[proc_index].init_time < time_passed) {
            proc_queue_add(&sim_queue, order[proc_index++]);
        }
        // Simulates process in the front and cycles to the back
        if (strcmp(sim_queue->head->name, "__IDLE__") == 0) {
            if (sim_queue->head != sim_queue->tail) {
                proc_queue_cycle(&sim_queue);
            }
            else {
                // Nothing to simulate, IDLES until next one reaches
                amount_context_change++;
                simulate(order[proc_index].init_time - time_passed, sim_queue->head, &time_passed);
                proc_queue_add(&sim_queue, order[proc_index++]);
            }
        }
        else {
            // Simulates 1 QUANTUM for this process
            amount_context_change++;
            if (sim_queue->head->time_started == -1) {
                sim_queue->head->time_started = time_passed;
                wait_time += time_passed - sim_queue->head->init_time;
            }
            simulate(quantum, sim_queue->head, &time_passed);
            amount_context_change++;
            if (sim_queue->head->dt == 0) {
                // process completed, removes process from queue
                debug_print(1, "Process %s finished", sim_queue->head->name);
                if ((time_passed - sim_queue->head->deadline) < 1e-4) {
                    amount_completed_in_deadline++;
                    debug_print(2, "Process %s finished within deadline.");
                }
                else {
                    float delta = time_passed - sim_queue->head->deadline;
                    debug_print(2, "Process %s finished, %fs after deadline", sim_queue->head->name, delta);
                    deadline_trespass += delta;
                }
                pop_head(&sim_queue);
            }
            else {
                proc_queue_cycle(&sim_queue);
            }
        }
        debug_print(0, "time_passed = %lf", time_passed);
    }
    debug_print(2, "Simulation complete.");
    float avg_wait_time = wait_time / size;
    float avg_deadline_trespass = deadline_trespass / size;
    printf("REPORT:\n\t Amount completed on deadline: %d\n\t\t\t\tTotal: %d\n\t\t\t\tRatio: %.2f%%\n",
                    amount_completed_in_deadline, size, (float) amount_completed_in_deadline * 100 / size);
    printf("\t    Amount of context changes: %lld\n\t\t    Average wait time: %f\n\t    Average deadline trespass: %f\n",
                amount_context_change, avg_wait_time, avg_deadline_trespass);
}

void shortest_job_first(deque **proc_queue) {

}

void priority_schedule(deque **proc_queue, float quantum) {

}
