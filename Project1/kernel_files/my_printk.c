#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage void sys_my_printk(int pid, long unsigned start_s, long unsigned start_ns,long unsigned end_s, long unsigned end_ns)
{
	printk("[project1] %d %lu.%09lu %lu.%09lu\n", pid, start_s, start_ns, end_s, end_ns);
	return;
}
