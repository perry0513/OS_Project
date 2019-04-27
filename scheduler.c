#define _GNU_SOURCE
#include "process.h"
#include "scheduler.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>

#define  rep(start, n)  for(int i = start; i < n; ++i)

/* current unit time */
static int currt;

/* currently running process, -1 if none */
static int running;

/* start time of last RR quantum */
static int rrt;

/* number of ended process */
static int finish;

/* process just finished */
static int just_finished;

/* index of first process that may be ready */
static int last_ready_proc;

/* number of process that are ready */
static int num_ready;

/* return next running process */
int next_process (struct process* proc, int num_proc, int policy) {

	/* non-preemptive */
	if(running != -1 && (policy == SJF || policy == FIFO))
		return running;
		
	int ret = -1;
	switch(policy){
		case PSJF:
		case SJF:
			rep(0, num_proc) {
				if (proc[i].pid == -1 || proc[i].t_exec == 0)
					continue;
				if (ret == -1 || proc[i].t_exec < proc[ret].t_exec)
					ret = i;
			}
			break;

		case FIFO:
			if(proc[just_finished+1].pid != -1) ret = just_finished + 1;
			break;

		case RR:
			int possibly_next = -1;
			/* if last process is done, assign next process */
			if (running == -1) possibly_next = just_finished + 1;
			/* if time quantum expires */
			else if ((ntime - t_last) % 500 == 0) possibly_next = running + 1;
			
			if (possibly_next != -1) {
				ret = possibly_next % num_proc;
				while (proc[ret].pid == -1 || proc[ret].t_exec == 0)
					ret = (ret + 1) % num_proc;
			}
			else ret = running;

		default: break;
	}
	return ret;

}

int compare (const void* a, const void* b) {
	return (struct process*)a - (struct process*)b;
}

void schedule (struct process* proc, int num_proc, int policy) {
	/* init */
	currt = 0;
	running = -1;
	lastrrt = 0;
	finish = 0;
	last_ready_proc = 0;
	just_finished = -1;
	num_ready = 0;

	/* Set single core prevent from preemption */
	proc_assign_cpu(getpid(), PARENT_CPU);
	
	/* Set high priority to scheduler */
	proc_wakeup(getpid());

	/* sort processes by ready time */
	qsort(proc, num_proc, sizeof(struct process), compare);

	/* assign all process pid to -1, i.e. not ready */
	rep (0, num_proc) {
		proc[i].pid = -1;
	}

	while (1) {

		/* Check if current running process is done */
		if (running != -1 && proc[running].t_exec == 0) {
			waitpid(proc[running].pid, NULL, 0);
			just_finished = running;
			running = -1;
			++finish;
			--num_ready;

			/* break if all process are done */
			if (finish == num_proc) break;
		}

		/* Check if processes are ready
		   If ready, assign pid and block process */
		rep(last_ready_proc, num_proc) {
			if (proc[i].t_ready == currt) {
				proc[i].pid = proc_exec(proc[i]);
				proc_block(proc[i].pid);
				++num_ready;
			} else {
				last_ready_proc = i;
				break;
			}
		}

		/* If there are process that are ready, select next process */
		if(num_ready){
			int next = next_process(proc, num_proc, policy);
			assert(next != -1);
			if (next != -1 && running != next) {
				/* Context switch */
				proc_wakeup(proc[next].pid);
				proc_block(proc[running].pid);
				running = next;
				rrt = currt;
			}
		}
		else {
			assert(running == -1);
		}

		
		/* Run one unit of time */
		run_unit_time();

		/* Minus current remaining time of running process by 1 */
		if (running != -1)
			--proc[running].t_exec;
		/* Increment current time by one */
		++currt;
	}

}