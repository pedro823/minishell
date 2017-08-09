/*
    Auxiliar functions
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */

#include <string.h>
#include <stdlib.h>
// TODO remove stdio
#include <stdio.h>
#include "error_handler.h"
#include "aux.h"

const string_vector split(const char *to_split, char* divisors) {
    add_to_stack("date->split");
    int divisor_count = 0;
    // Counts how many divisors are there
    for (int j = 0; j < strlen(divisors); j++) {
        char divisor = divisors[j];
        for (int i = 0; i < strlen(to_split); i++) {
            if (to_split[i] == divisor) {
                divisor_count++;
            }
        }
    }
    // Adds the space for the last string
    divisor_count++;
    string_vector v;
    v.size = divisor_count;
    v.data = emalloc(divisor_count * sizeof(char *));

    char *runner = strdup(to_split);
    // So that it can free runner
    char *start = runner;
    char *splitted_string;
    int counter = 0;

    while (runner != NULL) {
        splitted_string = strdup(strsep(&runner, divisors));
        // TODO check if this 'if' is really necessary
        if (strcmp(splitted_string, "") == 0) {
            v.data[counter++] = strdup("");
        }
        else {
            v.data[counter++] = splitted_string;
        }
    }
    free(start);
    pop_stack();
    return v;
}

void free_vector(string_vector to_free) {
    for (int i = 0; i < to_free.size; i++) {
        free(to_free.data[i]);
    }
    free(to_free.data);
}
