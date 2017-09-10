/*
    Auxiliar functions for schedulers
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
#include <pthread.h>
#include <math.h>

static int run = 1;

float minf(float a, float b) {
    return a < b ? a : b;
}

// Transforms the linked list into a vector. count is set through reference
// DESTROYS ORIGINAL LIST, SO NO FREE NECESSARY
struct process *to_array(deque **list, int *size) {
    add_to_stack("scheduler_methods->to_array");
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

deque *to_linked_list(struct process *array, int size) {
    add_to_stack("scheduler_methods->to_linked_list");
    deque *new_queue = NULL;
    if (array == NULL) {
        die_with_msg("No array found: *array is NULL");
    }
    for (int i = 0; i < size; i++) {
        proc_queue_add(&new_queue, array[i]);
    }
    free(array);
    pop_stack();
    return new_queue;
}

// Returns priority of the process. ALWAYS >= 1.
int get_priority(struct process proc, float time_now) {
    float ratio = (time_now + proc.dt) / proc.deadline;
    debug_print(0, "get_priority ratio = %f", ratio);
    return (int) ceil(ratio * 10);
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
    else {
        while (i <= mid && j <= end) {
            if (job_list[i].dt > job_list[j].dt) {
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
void sort_queue(struct process *job_list, int start, int end, char mode) {
    if (start < end) {
        int mid = (start + end) / 2;
        sort_queue(job_list, start, mid, mode);
        sort_queue(job_list, mid + 1, end, mode);
        merge(job_list, start, mid, end, mode);
    }
}

void generate_trace(deque **dead_proc_queue, struct scheduler_return statistics, char *name) {
    add_to_stack("generate_trace");
    node next;
    char filename[100];
    for (int i = 0; i < 100; i++) {
        filename[i] = '\0';
    }
    strncat(filename, "trace_", 100 - strlen(filename));
    strncat(filename, name, 100 - strlen(filename));
    strncat(filename, "_", 100 - strlen(filename));
    time_t t = time(NULL) % 100000;
    char t_string[30];
    snprintf(t_string, 30, "%lld", t);
    strncat(filename, t_string, 100 - strlen(filename));
    if (debug_priority_lte(3)) {
        fprintf(stderr, "\tThe report for this schedule is available at %s\n", filename);
    }

    FILE *stream = fopen(filename, "w");
    if (stream == NULL) {
        die_with_msg("File %s could not be open for writing.", filename);
    }

    for (node i = (*dead_proc_queue)->head; i != NULL; i = next) {
        next = i->next;
        fprintf(stream, "%s %.1f %.1f\n", i->name, i->time_started, i->time_ended - i->time_started);
        free(i);
    }
    fprintf(stream, "%d\n", statistics.amount_context_change);
    fclose(stream);
    pop_stack();
}

void print_statistics(struct scheduler_return *statistics, int array_size) {
    fprintf(stderr, "REPORT:\n\t Amount completed on deadline: %d\n\t\t\t\tTotal: %d\n\t\t\t\tRatio: %.2f%%\n",
    statistics->amount_completed_in_deadline, array_size, (float) statistics->amount_completed_in_deadline * 100 / array_size);
    fprintf(stderr, "\t    Amount of context changes: %lld\n\t\t    Average wait time: %.3f\n\t    Average deadline trespass: %.3f\n",
    statistics->amount_context_change, statistics->wait_time / array_size, statistics->deadline_trespass / array_size);
    fprintf(stderr, "     Total time consumed by scheduler: %.2lfs", statistics->total_time_scheduler);
    fprintf(stderr, "\n\n\tAverage wait time: Average time each process, after creation, waited to receive its first quantum\n");
    fprintf(stderr, "\tAverage deadline trespass: Average time that each process ran after its own deadline.\n");
}
