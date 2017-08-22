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

node read_process_list(char *file_name) {
    add_to_stack("read_process_list");
    FILE *stream = fopen(file_name, "r");
    if (stream == NULL) {
        die_with_msg("File %s could not be open for reading.", file_name);
    }

    char *line = NULL;
    int line_size;
    int count = 1;

    do {
        ssize_t size = getline(&line, &line_size, stream);
        if (size < 0) {
            die_with_msg("Error in getline(): size < 0");
        }
        string_vector args = __split(line, " ");
        if (args.size < 4) {
            die_with_msg("line no. %d has less than 4 arguments:\n\t%s", count, line);
        }

        count++;
    } while (line != NULL);


    pop_stack();
}
