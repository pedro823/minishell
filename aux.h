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
const string_vector split(const char *to_split, char* divisor);

/**
 * frees a string_vector.
 * @GUARANTEE no memory leaks possible.
 */
void free_vector(string_vector to_free);

#endif
