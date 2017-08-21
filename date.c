/******************************************************************************
 *                                                                            *
 *                   MAC0422 - Sistemas Operacionais - EP1                    *
 *                                                                            *
 *                           Displays current date                            *
 *                                                                            *
 *                      Pedro Pereira     - NUSP 9778794                      *
 *                      Raphael R. Gusmao - NUSP 9778561                      *
 *                                                                            *
 ******************************************************************************/

#include "date.h"
#include <stdio.h>
#include <time.h>

int date ()
{
    char buffer[29];
    time_t cur_time = time(NULL);
    strftime(buffer, 29, "%a %b %d %H:%M:%S %Z %Y", localtime(&cur_time));
    printf("%s\n", buffer);
    return(0);
}
