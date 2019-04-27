#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "process.h"
#include "scheduler.h"

/* current unit time */
static int currt;

/* currently running process, -1 if none */
static int running;

/* end time of last RR quantum */
static int lastrrt;

/* number of ended process */
static int finish;


int next_process (struct process* proc, int num_proc, int policy) {

}

int compare (const void* a, const void* b) {
	return (struct process*)a - (struct process*)b;
}

void schedule (struct process* porc, int num_proc, int policy) {
	/* init */
	currt = 0;
	running = -1;
	lastrrt = 0;
	finish = 0;

	qsort(proc, num_proc, sizeof(struct process), compare);



	while (1) {
		
	}

}
