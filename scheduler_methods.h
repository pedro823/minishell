/*
    Choices for EP1 part 2
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#ifndef __SCHEDULER_METHODS_H__
#define __SCHEDULER_METHODS_H__

#include "process_queue.h"

/**
 * Applies round_robin scheduling into proc_queue
 * @param proc_queue the processes to run
 * @param quantum    the minimum amount of time of continuous program running
 */
void round_robin(deque **proc_queue, float quantum);

/**
 * Completes the shortest job first in proc_queue
 * @param proc_queue the processes to run
 */
void shortest_job_first(deque **proc_queue);

/**
* Applies priority scheduling in proc_queue
 * @param proc_queue the processes to be run
 * @param quantum    the minimum amount of time of continuous program running
 */
void priority_schedule(deque **proc_queue, float quantum);

/**
 * Converts float to timespec
 */
struct timespec ftots(float dt);


#endif
