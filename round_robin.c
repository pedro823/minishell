/*
    Choices for EP1 part 2
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#include "round_robin.h"
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

// Visible to all threads
static deque *sim_queue;
static bool end_of_simulation; // Is the simulation over?
static pthread_mutex_t deque_flag;
static deque *dead_proc_queue; // All the ended processes


float get_current_time(clock_t starting_time) {
    return (float) (clock() - starting_time) / CLOCKS_PER_SEC;
}

// Function that is run at every simulation thread
void *simulator_thread(void *void_args) {
    // struct process *proc, float quantum, int count
    // check process_queue.h
    struct sim_arguments *args = (struct sim_arguments *) void_args;
    struct process *proc = emalloc(sizeof(struct process));
    *proc = *(args->proc);
    debug_print(0, "Process %s created", proc->name);
    while (true) {
        if (sem_wait(proc->entry_flag) != 0) {
            perror("semaphore error:");
            die_with_msg("Semaphore error in thread %s with %d return value.", proc->name, args->process_number);
        }
        float amount = minf(args->quantum, proc->dt);
        proc->dt -= amount;
        // Takes away rounding errors
        if (proc->dt < 1e-4) {
            proc->dt = 0;
        }
        if (sem_wait(proc->exit_flag) != 0) {
            perror("semaphor error:");
            die_with_msg("Semaphor error in thread %s with %d return value.", proc->name, args->process_number);
        }
        if (proc->dt == 0) {
            struct simulator_return *ret = emalloc(sizeof(struct simulator_return));
            ret->process_number = args->process_number;
            ret->time_ended = clock();
            free(proc);
            free(args);
            return (void *) ret;
        }
    }
}

// Arguments for queue_controller
struct queue_args {
    struct process *processes;
    int processes_size;
    float quantum;
};

// Thread that controls the entry of new processes
void *queue_controller(void *void_args) {
    clock_t time_start = clock();
    struct queue_args *args = (struct queue_args *) void_args;

    int count = 0;
    while (count < args->processes_size) {
        float clock_now = get_current_time(time_start);
        while (count < args->processes_size && clock_now >= args->processes[count].init_time) {
            debug_print(0, "queue_controller is now putting process '%s' in sim_queue", args->processes[count].name);
            sem_init(args->processes[count].entry_flag, 0, 0);
            sem_init(args->processes[count].exit_flag, 0, 0);
            args->processes[count].time_started = clock_now;
            struct sim_arguments *sim = emalloc(sizeof(struct sim_arguments));
            sim->quantum = args->quantum;
            sim->process_number = count;
            pthread_mutex_lock(&deque_flag);
            /* --- Enter critical section --- */
            proc_queue_add(&sim_queue, args->processes[count]);
            sim->proc = sim_queue->head;
            create_thread(sim_queue->head, simulator_thread, sim);
            /* --- Exit critical section  --- */
            pthread_mutex_unlock(&deque_flag);
            count++;
        }
    }
    end_of_simulation = true;
    return NULL;
}

// Round robin args
struct round_robin_args {
    float quantum;
};

// Scheduler thread for round robin
void *round_robin_simulator(void *void_args) {
    struct round_robin_args *args = (struct round_robin_args *) void_args;
    clock_t time_start = clock();
    float ideal_time = 0;
    struct scheduler_return *ret = emalloc(sizeof(struct scheduler_return));
    ret->wait_time = ret->deadline_trespass = 0;
    ret->amount_context_change = 0;
    while (!(end_of_simulation && is_empty(sim_queue))) {
        if (is_empty(sim_queue)) {
            // it's empty. sleep for 0.1 second.
            struct timespec sleep_for = ftots(0.1);
            if (nanosleep(&sleep_for, NULL) < 0) {
                add_to_stack("round_robin_simulator");
                die_with_msg("nanosleep failed.");
            }
            debug_print(0, "Idled for 0.1 second.");
            ideal_time += 0.1;
        }
        else {
            // Simulates HEAD
            // DOES NOT NEED MUTEX because it is a read-only operation
            // on somewhere that will not change
            node to_simulate = sim_queue->head;
            if (to_simulate->time_started == -1) {
                to_simulate->time_started = get_current_time(time_start);
                ret->wait_time += to_simulate->time_started - to_simulate->init_time;
            }
            // If process needs less than 1 quantum, give it less
            float ideal_run_time = minf(args->quantum, to_simulate->dt);
            debug_print(0, "simulating process %s for %fs", to_simulate->name, ideal_run_time);
            to_simulate->dt -= ideal_run_time;
            ret->amount_context_change++;
            sem_post(to_simulate->entry_flag);

            struct timespec nanosleep_time = ftots(ideal_run_time);
            if (nanosleep(&nanosleep_time, NULL) < 0) {
                die_with_msg("nanosleep failed at process %s", to_simulate->name);
            }

            ret->amount_context_change++;
            sem_post(to_simulate->exit_flag);

            pthread_mutex_lock(&deque_flag);
            /* --- Enter critical section --- */
            debug_print(0, "%s->dt = %f", to_simulate->name, to_simulate->dt);
            if (to_simulate->dt < 1e-4) {
                // Process ended. Remove and add to dead
                to_simulate->time_ended = get_current_time(time_start);
                float trespass = get_current_time(time_start) - to_simulate->deadline;
                if (trespass < 1e-3) {
                    ret->amount_completed_in_deadline++;
                }
                else {
                    ret->deadline_trespass += trespass;
                }
                proc_queue_add(&dead_proc_queue, pop_head(&sim_queue));
            }
            else {
                proc_queue_cycle(&sim_queue);
            }
            /* --- Exit critical section  --- */
            pthread_mutex_unlock(&deque_flag);
            ideal_time += ideal_run_time;
        }
    }
    ret->total_time_scheduler = (long double) ((clock() - time_start) / CLOCKS_PER_SEC) - ideal_time;
    debug_print(2, "If round robin took no processor time, this program should've ended in %.2fs", ideal_time);
    return (void *) ret;
}

// Wrapper & statistical analysis for round_robin
void round_robin(deque **proc_queue, float quantum) {
    add_to_stack("round_robin");
    sim_queue = emalloc(sizeof(deque));
    sim_queue->head = sim_queue->tail = NULL;
    pthread_t scheduler, controller;
    int size_order;
    struct process *order = to_array(proc_queue, &size_order);
    sort_queue(order, 0, size_order - 1, 0);
    // Invoke scheduler
    struct round_robin_args *scheduler_args = emalloc(sizeof(struct round_robin_args));
    scheduler_args->quantum = quantum;
    if (pthread_create(&scheduler, NULL, round_robin_simulator, (void *) scheduler_args) != 0) {
        die_with_msg("Failed to create round robin scheduler");
    }
    // Invoke controller
    struct queue_args *controller_args = emalloc(sizeof(struct queue_args));
    controller_args->processes = order;
    controller_args->processes_size = size_order;
    controller_args->quantum = quantum;
    if (pthread_create(&controller, NULL, queue_controller, (void *) controller_args) != 0) {
        die_with_msg("Failed to create round robin controller");
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
    pop_stack();
}
