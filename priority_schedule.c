/*
    Priority scheduling in EP1
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#include "priority_schedule.h"
#include "process_queue.h"
#include "scheduler_methods.h"
#include "_aux.h"
#include "error_handler.h"
#include <stdio.h>
#include <time.h>
#include <pthread.h>

// Visible to all threads
static deque *sim_queue;
static bool end_of_simulation;
static pthread_mutex_t deque_flag;
static deque *dead_proc_queue;

float ps_get_current_time(clock_t starting_time) {
    return (float) (clock() - starting_time) / CLOCKS_PER_SEC;
}

void *ps_simulator_thread(void *void_args) {
    struct sim_arguments *args = (struct sim_arguments *) void_args;
    struct process *proc = emalloc(sizeof(struct process));
    *proc = *(args->proc);
    debug_print(0, "Process %s created", proc->name);
    while (true) {
        if (sem_wait(proc->entry_flag) != 0) {
            perror("semaphore error:");
            die_with_msg("Semaphore error in thread %s", proc->name);
        }
        float amount = minf(args->quantum, proc->dt);
        proc->dt -= amount;
        // Takes away rounding errors
        if (proc->dt < 1e-4) {
            proc->dt = 0;
        }
        if (sem_wait(proc->exit_flag) != 0) {
            perror("semaphore error:");
            die_with_msg("Semaphor error in thread %s", proc->name);
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

// Arguments for priority scheduling controller
struct ps_controller_args {
    struct process *processes;
    int processes_size;
    float quantum;
};

// Controller for the entry of new processes
void *ps_controller(void *void_args) {
    clock_t time_start = clock();
    struct ps_controller_args *args = (struct ps_controller_args *) void_args;
    int count = 0;
    while (count < args->processes_size) {
        float clock_now = ps_get_current_time(time_start);
        while (count < args->processes_size && clock_now >= args->processes[count].init_time) {
            debug_print(0, "ps_controller is putting process '%s' in queue", args->processes[count].name);
            sem_init(args->processes[count].entry_flag, 0, 0);
            sem_init(args->processes[count].exit_flag, 0, 0);
            args->processes[count].time_started = clock_now;
            struct sim_arguments *sim = emalloc(sizeof(struct sim_arguments));
            sim->quantum = args->quantum;
            sim->process_number = count;
            pthread_mutex_lock(&deque_flag);
            /* --- Enter critical section --- */
            proc_queue_add(&sim_queue, args->processes[count]);
            sim->proc = sim_queue->tail;
            create_thread(sim_queue->tail, ps_simulator_thread, sim);
            /* --- Exit critical section  --- */
            pthread_mutex_unlock(&deque_flag);
            count++;
        }
    }
    end_of_simulation = true;
    return NULL;
}

// Priority schedule args
struct ps_args {
    float quantum;
};

void *priority_schedule_simulator(void *void_args) {
    struct ps_args *args = (struct ps_args *) void_args;
    clock_t time_start = clock();
    float ideal_time = 0;
    struct scheduler_return *ret = emalloc(sizeof(struct scheduler_return));
    ret->wait_time = ret->deadline_trespass = 0;
    ret->amount_context_change = 0;
    while (!end_of_simulation || !is_empty(sim_queue)) {
        if (is_empty(sim_queue)) {
            // Queue is empty. IDLES.
            struct timespec sleep_for = ftots(0.1);
            if (nanosleep(&sleep_for, NULL) < 0) {
                add_to_stack("round_robin_simulator");
                die_with_msg("Nanosleep failed");
            }
            debug_print(0, "Idled for 0.1s");
            ideal_time += 0.1;
        }
        else {
            // Gets process to simulates and does it
            node to_simulate = sim_queue->head;
            if (to_simulate->time_started == -1) {
                to_simulate->time_started = ps_get_current_time(time_start);
                ret->wait_time += to_simulate->time_started - to_simulate->init_time;
            }
            // Finds how many quantums this process will have
            int amount_quantum = get_priority(*to_simulate);
            // Gets minimum between amount of quantum vs. dt
            float ideal_run_time = minf(args->quantum * amount_quantum, to_simulate->dt);
            debug_print(0, "Priority for %s is %d quantums", to_simulate->name, amount_quantum);
            ret->amount_context_change++;
            sem_post(to_simulate->entry_flag);

            struct timespec nanosleep_time = ftots(ideal_run_time);
            if (nanosleep(&nanosleep_time, NULL) < 0) {
                add_to_stack("round_robin_simulator");
                die_with_msg("Nanosleep failed");
            }
            to_simulate->dt -= ideal_run_time;
            ret->amount_context_change++;
            sem_post(to_simulate->exit_flag);
            pthread_mutex_lock(&deque_flag);
            /* --- Enter critical section --- */
            debug_print(0, "%s->dt = %.1f", to_simulate->name, to_simulate->dt);
            if (to_simulate->dt < 1e-4) {
                // Process ended.
                to_simulate->time_ended = ps_get_current_time(time_start);
                float trespass = ps_get_current_time(time_start) - to_simulate->deadline;
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
    debug_print(2, "If Priority Scheduling took no processor time, this program would've ended in %.2fs", ideal_time);
    return (void *) ret;
}

// Wrapper & statistical analysis for Priority scheduling
void priority_schedule(deque **proc_queue, float quantum) {
    add_to_stack("priority_schedule");
    sim_queue = emalloc(sizeof(deque));
    sim_queue->head = sim_queue->tail = NULL;
    pthread_t scheduler, controller;
    int size_order;
    struct process *order = to_array(proc_queue, &size_order);
    sort_queue(order, 0, size_order - 1, 0);
    // Invoke scheduler
    struct ps_args *scheduler_args = emalloc(sizeof(struct ps_args));
    scheduler_args->quantum = quantum;
    if (pthread_create(&scheduler, NULL, priority_schedule_simulator, (void *) scheduler_args) != 0) {
        die_with_msg("Failed to create priority scheduler");
    }
    // Invoke controller
    struct ps_controller_args *controller_args = emalloc(sizeof(struct ps_controller_args));
    controller_args->processes = order;
    controller_args->processes_size = size_order;
    controller_args->quantum = quantum;
    if (pthread_create(&controller, NULL, ps_controller, (void *) controller_args) != 0) {
        die_with_msg("Failed to create priority controller");
    }
    // Joins with controller
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
