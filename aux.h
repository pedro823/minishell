/*
    Auxiliar functions
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#ifndef __AUX_H__
#define __AUX_H__

#define bool char
#define true 1
#define false 0
#define MAX_LENGTH_CONSTANT 2048

typedef struct {
     int size;
     char **data;
} string_vector;

/**
* just like python's split: splits the string into a vector of instances before and after
* the divisor.
* @param  to_split The string to be splitted.
* @param  divisors  The divisors of the string
* @GUARANTEE No memory leaks possible
* @return_example  split("abc=d;e", "=.;") => string_vector { size: 3, data: ["abc", "d", "e"] }
*
*/
const string_vector __split(const char *to_split, char* divisor);

/**
 * Forks and runs the path
 * @param  directory_path [description]
 * @return                The exit code of the executed program
 */
int __exec(char *path, const char* line);

/**
* Finds the working directory of the shell.
* @return  the working directory
*/
char *__find_wd(void);

/**
 * Finds the executable. if it starts with ./, finds in working directory
 * @param       name of the executable
 * @return      the path to the executable, NULL if not found
 */
char *__find(const char *name);

/**
 * Finds the executable in PATH and executes
 * @param name The name of the program
 */
int __find_exec(const char *line);

/**
 * frees a string_vector.
 * @GUARANTEE no memory leaks possible.
 */
void free_vector(string_vector to_free);

/**
 * reset an array of chars of MAX_LENGTH_CONSTANT size
 * @param to_reset the char array to be reset
 */
void __reset_char_array(char *to_reset);

#endif
