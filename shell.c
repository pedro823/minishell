/*
    Razgriz Shell
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */

#include <stdio.h>
#include <readline/history.h>
#include <stdlib.h>
#include "error_handler.h"
#include "chown.h"
#include "date.h"
#include "assert.h"

/**
 * Finds the executable in PATH and executes
 * @param name The name of the program
 */
void __exec (const char *name) {
    add_to_stack("__exec");
    const char *path = secure_getenv("PATH");
    // How many directories are there in the PATH?
    int path_colons = 0;
    for (int i = 0; i < strlen(path); i++) {
        if (path[i] == ':') {
            path_colons++;
        }
    }
    char **path_directories = emalloc(path_colons * sizeof(char *));

    for (int i = 0; i < path_colons; i++) {

    }

    pop_stack();
}


int main (int argc, const char **argv) {
    add_to_stack("main");
    set_debug_priority(0);
    set_program_name("ep1sh");

}
