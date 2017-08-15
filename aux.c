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
#include <unistd.h>
#include <readline/readline.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "error_handler.h"
#include "aux.h"

const string_vector __split(const char *to_split, char* divisors) {
    add_to_stack("date->split");
    int divisor_count = 0;
    // Counts how many divisors are there
    int len_divisors = strlen(divisors);
    int len_to_split = strlen(to_split);
    for (int j = 0; j < len_divisors; j++) {
        char divisor = divisors[j];
        for (int i = 0; i < len_to_split; i++) {
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
    add_to_stack("aux->free_vector");
    for (int i = 0; i < to_free.size; i++) {
        free(to_free.data[i]);
    }
    free(to_free.data);
    pop_stack();
}

void __reset_char_array(char *to_reset) {
    for (int i = 0; i < MAX_LENGTH_CONSTANT; i++) {
        to_reset[i] = '\0';
    }
}

int __exec(char *path, const char* line) {
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        // CHILD
        string_vector v = __split(line, " ");
        free(v.data[0]);
        char **data = emalloc((v.size + 1) * sizeof(char *));
        data[0] = path;
        for (int i = 1; i < v.size; i++) {
            data[i] = v.data[i];
        }
        data[v.size] = (char *) NULL;
        execv(data[0], data);
        perror("failed to execute");
        debug_print(0, "Failed to execute command %s", path);
        exit(-1);
    }
    else {
        // PARENT
        int status = 0;
        pid_t finished = wait(&status);
        return status;
    }
}

char *__find_wd(void) {
    add_to_stack("__find_wd");
    char working_directory[MAX_LENGTH_CONSTANT];
    if (getcwd(working_directory, MAX_LENGTH_CONSTANT) == NULL) {
        die_with_msg("__find_wd: working directory error");
    }
    pop_stack();
    return strdup(working_directory);
}

char *__find(const char *name) {
    add_to_stack("__find");
    char *file_path;
    if (name[0] == '/') {
        if (access(name, F_OK) == -1) {
            pop_stack();
            return NULL;
        }
        file_path = strdup(name);
    }
    else if (name[0] == '.' && name[1] == '/') {
        char *wd = __find_wd();
        wd = strncat(wd, "/", MAX_LENGTH_CONSTANT - strlen(wd));
        wd = strncat(wd, &(name[2]), MAX_LENGTH_CONSTANT - strlen(wd));
        file_path = strdup(wd);
        debug_print(0, "file_path: %s", file_path);
        if (strlen(file_path) == MAX_LENGTH_CONSTANT) {
            die_with_msg("path to file is too large. Recompile with bigger MAX_LENGTH_CONSTANT");
        }
        if (access(file_path, F_OK) == -1) {
            pop_stack();
            return NULL;
        }
    }
    else {
        char test_path[MAX_LENGTH_CONSTANT];
        __reset_char_array(test_path);
        const char *path = getenv("PATH");
        string_vector paths = __split(path, ":");
        for (int i = 0; i < paths.size; i++) {
            strncat(test_path, paths.data[i], MAX_LENGTH_CONSTANT);
            strncat(test_path, "/", MAX_LENGTH_CONSTANT - strlen(test_path));
            strncat(test_path, name, MAX_LENGTH_CONSTANT - strlen(test_path));
            if (strlen(test_path) == MAX_LENGTH_CONSTANT) {
                die_with_msg("path to file is too large. Recompile with bigger MAX_LENGTH_CONSTANT");
            }
            if (access(test_path, F_OK) != -1) {
                file_path = strdup(test_path);
                break;
            }
            __reset_char_array(test_path);
        }
        free_vector(paths);
        if (strcmp(test_path, "\0") == 0) {
            pop_stack();
            return NULL;
        }
    }
    pop_stack();
    return file_path;
}

int __find_exec(const char *line) {
    add_to_stack("__find_exec");
    string_vector split_line = __split(line, " ");
    char *path = __find(split_line.data[0]);
    free_vector(split_line);
    debug_print(0, "path = %s", path);
    if (path == NULL) {
        printf("%s: Program not found or permissions not granted\n", split_line.data[0]);
        pop_stack();
        return -2;
    }
    else {
        debug_print(0, "Executing %s", path);
        int result = __exec(path, line);
        free(path);
        pop_stack();
        return result;
    }
}
