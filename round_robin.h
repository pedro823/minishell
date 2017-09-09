/*
    Choices for EP1 part 2
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#ifndef __ROUND_ROBIN_H__
#define __ROUND_ROBIN_H__

#include "process_queue.h"

/**
 * Applies round_robin scheduling into proc_queue
 * @param proc_queue the processes to run
 * @param quantum    the minimum amount of time of continuous program running
 */
void round_robin(deque **proc_queue, float quantum);

#endif
