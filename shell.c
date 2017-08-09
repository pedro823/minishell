/*
    Razgriz Shell
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */

#include <stdio.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "error_handler.h"
#include "chown.h"
#include "date.h"

/**
 * Never to be exposed to main(). Forks and runs the path
 * @param  directory_path [description]
 * @return                [description]
 */
int __exec(char *path) {
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        // Is the child process, executes path
    }
}

/**
 * Returns $ if user, # if superuser, $x/#x if last program failed.
 * @param  last_program_status The status of the program last run.
 */
char* __dollar_sign(int last_program_status) {
    add_to_stack("__dollar_sign");
    uid_t user_id = getuid();
    if (user_id == 0) {
        if (last_program_status == 0) {
            pop_stack();
            return "# ";
        }
        else {
            pop_stack();
            return "#x "
        }
    }
    if (last_program_status == 0) {
        pop_stack();
        return "$ "
    }
    pop_stack();
    return "$x "
}

/**
 * Finds the working directory of the shell.
 * @return  the working directory
 */
char *__find_wd(void) {
    add_to_stack("__find_wd");
    char working_directory[MAX_LENGTH_CONSTANT];
    if (getcwd(working_directory, MAX_LENGTH_CONSTANT) != NULL) {
        
    }
    else {
        kill("__find_wd: working directory error");
    }
    pop_stack();
    return working_directory;
}

/**
 * Finds the executable. if it starts with ./, finds in working directory
 * @param       name of the executable
 * @return      the path to the executable, NULL if not found
 */
char *__find(const char *name) {
    add_to_stack("__find");
    if (name[0] == '.' && name[1] == '/') {
        char *wd = find_wd();
    }
    pop_stack();
}

/**
 * Finds the executable in PATH and executes
 * @param name The name of the program
 */
int __find_exec(const char *name) {
    add_to_stack("__find_exec");
    int result;
    const char *path = secure_getenv("PATH");
    // How many directories are there in the PATH?
    string_vector paths = split(path, ":");
    char *test_path;
    for (int i = 0; i < paths.size; i++) {
        test_path = strdup();
        if (access(test_path, X_OK) != -1) {
            break;
        }
        free(test_path);
        test_path = NULL;
    }

    if (test_path == NULL) {
        printf("%s: Program not found or permissions not granted\n", name);
        pop_stack();
        return -2;
    }
    else {
        result = __exec(test_path);
        pop_stack();
        return result;
    }
}


int main (int argc, const char **argv) {
    add_to_stack("main");
    set_debug_priority(0);
    set_program_name("ep1sh");
    char *line;
    bool stop = false;
    int last_program_status = 0;
    while (!stop) {

    }
}
