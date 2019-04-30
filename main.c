#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include "process.h"
#include "scheduler.h"

int main()
{

	char sched_policy[4];
	int policy;
	int nproc;
	struct process *proc;

	scanf("%4s", sched_policy);
	
	if (strcmp(sched_policy, "FIFO") == 0) {
		policy = FIFO;
	}
	else if (strcmp(sched_policy, "RR") == 0) {
		policy = RR;
	}
	else if (strcmp(sched_policy, "SJF") == 0) {
		policy = SJF;
	}
	else if (strcmp(sched_policy, "PSJF") == 0) {
		policy = PSJF;
	}
	else {
		fprintf(stderr, "Invalid policy: %s", sched_policy);
		exit(0);
	}

	if (scanf("%d", &nproc) < 1 || nproc < 0) {
		fprintf(stderr, "Invalid number of processes.(Or policy exceeds 4 characters)");
		exit(0);
	}
	
	proc = (struct process *)malloc(nproc * sizeof(struct process));

	for (int i = 0; i < nproc; ++i) {
		scanf("%32s", proc[i].name);
		if (scanf("%d%d", &proc[i].t_ready, &proc[i].t_exec) < 2 || proc[i].t_ready < 0 || proc[i].t_exec < 0) {
			fprintf(stderr, "Invalid ready time or execution time.(Or process name exceeds 32 characters)");
			exit(0);
		}
	}

	schedule(proc, nproc, policy);

	// print pid
	for (int i = 0; i < nproc; ++i)
		fprintf(stderr, "%s %d\n", proc[i].name, proc[i].pid);

	exit(0);
}
