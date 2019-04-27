#ifndef SCHED_H
#define SCHED_H

#include "process.h"

#define FIFO 1
#define RR	 2
#define SJF  3
#define PSJF 4

void schedule(struct process *, int num_proc, int policy);

#endif