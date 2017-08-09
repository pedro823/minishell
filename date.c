/*
    Displays current date
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#include "date.h"
#include "error_handler.h"
#include <stdio.h>
#include <string.h>

// FIXME this is not necessary
typedef struct {
    char *date; // if NULL, get current date
    char *file; // if not NULL, prints dates from file
    bool display_version; // if true, displays version and exits
    char *set_time; // if not NULL, sets time to string
    /*
    Valid formats: {
        'iso-8601=hours'
        'iso-8601=minutes'
        'iso-8601=seconds'
        'iso-8601=nanoseconds'
        'rfc-email'
        'rfc-3339=date'
        'rfc-3339=seconds'
        'rfc-3339=ns'
    }
    */
    char *format;
} options;
