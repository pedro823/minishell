/*
    Scheduler for EP1 part 2
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */

#include <stdio.h>
#include "error_handler.h"
#include "_aux.h"
#include "process_queue.h"
#include "scheduler_methods.h"
#include "shortest_job_first.h"
#include "round_robin.h"
#include "priority_schedule.h"
#include <string.h>
#include <time.h>
#include <semaphore.h>

#define QUANTUM 0.1

deque *read_process_list(char *file_name) {
    add_to_stack("scheduler->read_process_list");
    FILE *stream = fopen(file_name, "r");
    if (stream == NULL) {
        die_with_msg("File %s could not be open for reading.", file_name);
    }

    char *line = NULL;
    size_t line_size;
    ssize_t size;
    int count = 1;
    deque* queue = NULL;

    do {
        size = getline(&line, &line_size, stream);
        if (size < 0) {
            break;
        }

        string_vector args = __split(line, " ");
        if (args.size < 4) {
            die_with_msg("line no. %d has less than 4 arguments:\n\t%s", count, line);
        }
        // Adds to proc_queue
        struct process p;
        p.init_time = atof(args.data[0]);
        p.dt = atof(args.data[1]);
        p.deadline = atof(args.data[2]);
        p.name = __chomp(strdup(args.data[3]));
        p.time_started = -1;
        p.entry_flag = emalloc(sizeof(sem_t));
        p.exit_flag = emalloc(sizeof(sem_t));

        proc_queue_add(&queue, p);
        free_vector(args);
        count++;
    } while (size >= 0);
    fclose(stream);

    pop_stack();
    return queue;
}

void print_usage(char *name) {
    printf("usage: %s <file> <1/2/3> <debug?>\n", name);
    printf("1 - Shortest Job First\n2 - Round Robin\n3 - Priority Scheduling");
    printf("For each d in <debug?>, debug_priority decreases by 1 (Start: 4, min: 0)\n");
}

int main(int argc, char **argv) {
    set_program_name("scheduler.c");
    add_to_stack("main");
    if (argc < 3 || argc > 4) {
        print_usage(argv[0]);
        return 1;
    }

    int debug_priority = 4;
    if (argc == 4) {
        for (char *i = argv[3]; *i != '\0'; i++) {
            if (*i == 'd') {
                debug_priority = (debug_priority <= 0 ? 0 : debug_priority - 1);
            }
        }
    }
    set_debug_priority(debug_priority);
    debug_print(3, "Debug mode. priority = %d", debug_priority);

    deque *pq = read_process_list(argv[1]);
    debug_print(1, "read process queue.");

    if (debug_priority_lte(0)) {
        print_proc_queue(pq);
    }

    if (strcmp(argv[2], "1") == 0) {
        shortest_job_first(&pq);
    }
    else if (strcmp(argv[2], "2") == 0) {
        round_robin(&pq, QUANTUM);
    }
    else if (strcmp(argv[2], "3") == 0) {
        priority_schedule(&pq, QUANTUM);
    }
    else {
        print_usage(argv[0]);
        die_with_msg("Second argument is not \"1\", \"2\" or \"3\"");
    }

    pop_stack();
    return 0;
}
