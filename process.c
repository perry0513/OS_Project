#define _GNU_SOURCE
#include "process.h"
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
#define GET_TIME 333
#define PRINTK 334

struct timespec start_t, end_t;

// assign process #pid to CPU #core
int proc_assign_cpu(int pid, int core)
{
	// printf("core = %d\n", core);
	if (core > sizeof(cpu_set_t)) {
		fprintf(stderr, "Core index error.");
		return -1;
	}

	cpu_set_t mask; // Implemented as bit mask
	CPU_ZERO(&mask); // Clears set, so that it contains no CPUs
	CPU_SET(core, &mask); // Add CPU cpu to set

	if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
		perror("sched_setaffinity");
		exit(1);
	}
	// printf("sched_getaffinity = ");
	// if (sched_getaffinity(pid, sizeof(mask), &mask) < 0) {
	// 	perror("sched_getaffinity");
	// 	exit(1);
	// }
	// else {
	// 	long nproc = sysconf(_SC_NPROCESSORS_ONLN);
	// 	for (int i = 0;i < nproc;i++){
	// 		printf("%d", CPU_ISSET(i, &mask));
	// 	}
	// 	printf("\n");
	// }

	return 0;
}

int proc_exec(struct process proc)
{
	int pid = fork();

	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		char to_dmesg[200];
		syscall(GET_TIME, &start_t);
		for (int i = 0; i < proc.t_exec; i++) {
			run_unit_time();
#ifdef DEBUG
			if (i % 100 == 0)
				fprintf(stderr, "%s: %d/%d\n", proc.name, i, proc.t_exec);
#endif
		}
		syscall(GET_TIME,&end_t);
		//sprintf(to_dmesg, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start_t.tv_sec, start_t.tv_nsec, end_t.tv_sec, end_t.tv_nsec);
		//syscall(PRINTK, to_dmesg);
		syscall(PRINTK, getpid(), start_t.tv_sec, start_t.tv_nsec, end_t.tv_sec, end_t.tv_nsec);

		exit(0);
	}
	
	/* Assign child to another core prevent from interupted by parant */
	proc_assign_cpu(pid, CHILD_CPU);

	return pid;
}

int proc_block(int pid)
{
	struct sched_param param;
	
	/* SCHED_IDLE should set priority to 0 */
	param.sched_priority = 0;

	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}

int proc_wakeup(int pid)
{
	struct sched_param param;
	
	/* SCHED_OTHER should set priority to 0 */
	param.sched_priority = 0;

	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}
