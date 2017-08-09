/*
    Displays current date
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */

#ifndef __DATE_H__
#define __DATE_H__

#include "aux.h"

// TODO remove comments from this struct
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


/**
 * Prints date and returns
 */
void __date(date_options options);

#endif
