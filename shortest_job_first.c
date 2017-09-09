#include "shortest_job_first.h"
#include "scheduler_methods.h"
#include "process_queue.h"
#include "error_handler.h"
#include "_aux.h"
#include <stdio.h>
#include <pthread.h>
#include <error.h>
#include <semaphore.h>
#include <time.h>
#include <math.h>

static deque *jobs;
static bool end_of_simulation; // Is the simulation over?
static bool queue_changed;
static pthread_mutex_t change_bools;
static pthread_mutex_t jobs_flag;
static deque *dead_proc_queue; // All the ended processes

float sjf_get_current_time(clock_t starting_time) {
    return (float) (clock() - starting_time) / CLOCKS_PER_SEC;
}

// Simulates a process in shortest job first
void *sjf_simulator(void *void_args) {
    struct sim_arguments *args = (struct sim_arguments *) void_args;
    debug_print(0, "Process %s created", args->proc->name);
    if (sem_wait(args->proc->entry_flag) != 0) {
        perror("semaphore error:");
        die_with_msg("Semaphore error in thread %s.", args->proc->name);
    }
    // Wait for ending...
    struct simulator_return *ret = emalloc(sizeof(struct simulator_return));
    if (sem_wait(args->proc->exit_flag) != 0) {
        perror("semaphore error:");
        die_with_msg("Semaphore error in thread %s.", args->proc->name);
    }
    debug_print(0, "Process %s ending...", args->proc->name);
    ret->process_number = args->process_number;
    ret->time_ended = clock();
    free(args);
    return (void *) ret;
}

struct sjf_queue_args {
    struct process *processes;
    int processes_size;
};

void *sjf_queue_controller(void *void_args) {
    struct sjf_queue_args *args = (struct sjf_queue_args *) void_args;
    pthread_mutex_lock(&change_bools);
    queue_changed = false;
    pthread_mutex_unlock(&change_bools);
    clock_t time_start = clock();
    int count = 0;
    while (count < args->processes_size) {
        float clock_now = sjf_get_current_time(time_start);
        while (count < args->processes_size && clock_now >= args->processes[count].init_time) {
            debug_print(0, "queue_controller is now putting process '%s' in queue", args->processes[count].name);
            sem_init(args->processes[count].entry_flag, 0, 0);
            sem_init(args->processes[count].exit_flag, 0, 0);
            args->processes[count].time_started = clock_now;
            struct sim_arguments *sim = emalloc(sizeof(struct sim_arguments));
            sim->process_number = count;
            pthread_mutex_lock(&change_bools);
            pthread_mutex_lock(&jobs_flag);
            /* --- Enter critical section --- */
            queue_changed = true;
            proc_queue_add(&jobs, args->processes[count]);
            sim->proc = jobs->head;
            create_thread(jobs->head, sjf_simulator, sim);
            /* --- Exit critical section  --- */
            pthread_mutex_unlock(&jobs_flag);
            pthread_mutex_unlock(&change_bools);
            count++;
        }
    }
    end_of_simulation = true;
    return NULL;
}

// Merges new processes with old processes
// DESTROYS ORIGINAL ARRAYS
struct process *sjf_merge_new(struct process *a, struct process *b, int size_a, int size_b) {
    if (a == NULL) {
        return b;
    }
    if (b == NULL) {
        return a;
    }
    struct process *merged = emalloc((size_a + size_b) * sizeof(struct process));
    int i = 0, j = 0, k = 0;
    while (i < size_a  && j < size_b) {
        if (a[i].dt > b[i].dt) {
            merged[k++] = a[i++];
        }
        else {
            merged[k++] = b[j++];
        }
    }
    while (i < size_a) {
        merged[k++] = a[i++];
    }
    while (j < size_b) {
        merged[k++] = b[j++];
    }
    free(a);
    free(b);
    return merged;
}

// Scheduler for shortest job first
void *sjf_job_simulator(void *void_args) {
    int size = 0;
    struct process *organized_jobs = NULL;
    struct scheduler_return *ret = emalloc(sizeof(struct scheduler_return));
    clock_t time_started = clock();
    while(!end_of_simulation || size > 0) {
        pthread_mutex_lock(&change_bools);
        int new_size;
        if (queue_changed) {
            // Adds new processes
            queue_changed = false;
            pthread_mutex_lock(&jobs_flag);
            struct process *new_processes = to_array(&jobs, &new_size);
            jobs = NULL;
            debug_print(0, "Adding %d new processes to list...", new_size);
            pthread_mutex_unlock(&jobs_flag);
            sort_queue(new_processes, 0, new_size - 1, 1);
            organized_jobs = sjf_merge_new(organized_jobs, new_processes, size, new_size);
            size += new_size;
        }
        pthread_mutex_unlock(&change_bools);
        if (size > 0) {
            // runs first from process list: size - 1
            // Organized_jobs is sorted in INVERTED order.
            struct process proc = organized_jobs[size - 1];
            size--;
            sem_post(proc.entry_flag);
            struct timespec sleep_amount = ftots(proc.dt);
            if (nanosleep(&sleep_amount, NULL) != 0) {
                die_with_msg("sjf_job_simulator: Nanosleep failed");
            }
            sem_post(proc.exit_flag);
            // proc is done. adds to dead list
            proc.time_ended = sjf_get_current_time(time_started);
            proc_queue_add(&dead_proc_queue, proc);
        }
        else {
            // No process to run: idles
            struct timespec sleep_for = ftots(0.1);
            if (nanosleep(&sleep_for, NULL) < 0) {
                add_to_stack("round_robin_simulator");
                die_with_msg("nanosleep failed.");
            }
            debug_print(0, "Idled for 0.1 second.");
        }
    }
    free(organized_jobs);
    return (void *) ret;
}

// Wrapper & statistical analysis for shortest_job_first
void shortest_job_first(deque **proc_queue) {
    add_to_stack("shortest_job_first");
    jobs = emalloc(sizeof(deque));
    jobs->head = jobs->tail = NULL;
    pthread_t scheduler, controller;
    int size_order;
    struct process *order = to_array(proc_queue, &size_order);
    jobs = emalloc(size_order * sizeof(struct process)); // Will always have enough space
    sort_queue(order, 0, size_order - 1, 0);
    // Invoke scheduler
    if (pthread_create(&scheduler, NULL, sjf_job_simulator, NULL) != 0) {
        die_with_msg("Failed to create shortest job first scheduler");
    }
    // Invoke controller
    struct sjf_queue_args *controller_args = emalloc(sizeof(struct sjf_queue_args));
    controller_args->processes = order;
    controller_args->processes_size = size_order;
    if (pthread_create(&controller, NULL, sjf_queue_controller, (void *) controller_args) != 0) {
        die_with_msg("Failed to create shortest job first controller");
    }
    // Wait for controller to finish (will always finish first)
    if (pthread_join(controller, NULL) != 0) {
        die_with_msg("Failed to join with controller");
    }
    struct scheduler_return *statistics = emalloc(sizeof(struct scheduler_return));
    if (pthread_join(scheduler, (void **) &statistics) != 0) {
        die_with_msg("Failed to join with scheduler");
    }
    if (debug_priority_lte(3)) {
        if (statistics == NULL) {
            fprintf(stderr, "statistics structure was not set\n");
            pop_stack();
            return;
        }
        print_statistics(statistics, size_order);
    }
    free(statistics);
    pop_stack();
}
