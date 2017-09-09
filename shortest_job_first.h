/*
    Shortest job first handler
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#ifndef __SHORTEST_JOB_FIRST_H__
#define __SHORTEST_JOB_FIRST_H__

#include "process_queue.h"

/**
* Completes the shortest job first in proc_queue
* @param proc_queue the processes to run
*/
void shortest_job_first(deque **proc_queue);

#endif
