/******************************************************************************
 *                                                                            *
 *                   MAC0422 - Sistemas Operacionais - EP1                    *
 *                                                                            *
 *                          Change Ownership of file                          *
 *                                                                            *
 *                      Pedro Pereira     - NUSP 9778794                      *
 *                      Raphael R. Gusmao - NUSP 9778561                      *
 *                                                                            *
 ******************************************************************************/

#include <unistd.h>
#include "chown.h"
#include "_aux.h"

int change_owner (const char *line)
{
    string_vector split = __split(line, " ");
    gid_t group = atoi(split.data[1]);
    char *file = split.data[2];
    int ret = chown(file, (uid_t)-1, group);
    free_vector(split);
    return ret;
}
