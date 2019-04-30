#ifndef SCHED_H
#define SCHED_H

#include "process.h"

enum Policy{FIFO, RR, SJF, PSJF};

void schedule(struct process *, int num_proc, int policy);

#endif
