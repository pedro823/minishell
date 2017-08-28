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
#include <string.h>

deque *read_process_list(char *file_name) {
    add_to_stack("scheduler->read_process_list");
    debug_print(0, "Hello!");
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
        debug_print(0, "count = %d", count);

        string_vector args = __split(line, " ");
        if (args.size < 4) {
            die_with_msg("line no. %d has less than 4 arguments:\n\t%s", count, line);
        }
        // Adds to proc_queue
        struct process p;
        p.init_time = atof(args.data[0]);
        p.dt = atof(args.data[1]);
        p.deadline = atof(args.data[2]);
        p.name = strdup(args.data[3]);

        proc_queue_add(&queue, p);
        print_proc_queue(queue);
        free_vector(args);
        count++;
    } while (size >= 0);
    fclose(stream);

    pop_stack();
    return queue;
}

int main(int argc, char **argv) {
    set_debug_priority(0);
    set_program_name("scheduler.c");
    if (argc != 2) {
        printf("usage: %s <file>", argv[0]);
        return 1;
    }
    deque *pq = read_process_list(argv[1]);
    print_proc_queue(pq);
    return 0;
}
