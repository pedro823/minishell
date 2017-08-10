/*
    Razgriz Shell
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */

#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "error_handler.h"
#include "chown.h"
#include "date.h"
#include "aux.h"

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
            return "#x ";
        }
    }
    if (last_program_status == 0) {
        pop_stack();
        return "$ ";
    }
    pop_stack();
    return "$x ";
}

/**
 * If it is one of our special programs, executes it.
 * Otherwise go to __find_exec
 * @param  line The line read in stdin
 * @return      the return status of the program.
 */
int execute(const char *line) {
    add_to_stack("execute");
    string_vector split_line = __split(line, " ");
    int ret;
    if (strcmp(split_line.data[0], "cd") == 0) {
        if (split_line.size > 1) {
            ret = chdir(split_line.data[1]);
        }
        else {
            ret = chdir("~");
        }
    }
    else if (strcmp(split_line.data[0], "date") == 0) {
        ret = date(line);
    }
    else if (strcmp(split_line.data[0], "chown") == 0) {
        ret = change_owner(line);
    }
    else {
        ret = __find_exec(line);
    }
    free_vector(split_line);
    pop_stack();
    return ret;
}


int main (int argc, const char **argv) {
    add_to_stack("main");
    set_debug_priority(0);
    set_program_name("ep1sh");
    char *line;
    bool stop = false;
    int last_program_status = 0;
    char prompt[MAX_LENGTH_CONSTANT];
    char *directory_path;
    char *dollar_sign;

    while (!stop) {
        __reset_char_array(prompt);
        prompt[0] = '[';
        directory_path = __find_wd();
        dollar_sign = __dollar_sign(last_program_status);
        debug_print(0, "pid: %d", getpid());
        strncat(prompt, directory_path, MAX_LENGTH_CONSTANT - strlen(prompt));
        strncat(prompt, "]", MAX_LENGTH_CONSTANT - strlen(prompt));
        strncat(prompt, dollar_sign, MAX_LENGTH_CONSTANT - strlen(prompt));
        line = readline(prompt);
        if (line == NULL) {
            stop = true;
        }
        else {
            last_program_status = execute(line);
        }
        free(line);
        free(directory_path);
    }
    printf("\n");
    return 0;
}
