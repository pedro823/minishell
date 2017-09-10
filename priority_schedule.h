/*
    Priority scheduling in EP1
    Created by:
    Pedro Pereira                   9778794
    Raphael Gusmão                  9778561
 */
#ifndef __PRIORITY_SCHEDULE_H__
#define __PRIORITY_SCHEDULE_H__

#include "process_queue.h"

/**
 * Applies priority scheduling into proc_queue
 * @param proc_queue the processes to run
 * @param quantum    the minimum amount of time of continuous program running
 */
void priority_schedule(deque **proc_queue, float quantum);

#endif
