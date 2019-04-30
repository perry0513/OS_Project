#include <linux/kernel.h>
#include <linux/linkage.h>

asmlinkage void sys_printk(char* msg)
{
	char buf[256];
	long copied = strncpy_from_user(buf, msg, sizeof(buf));
	printk(buf);
	return;
}

/*
SYSCALL_DEFINE1 (printk, char*, msg) {
	char buf[256];
	long copied = strncpy_from_user(buf, msg, sizeof(buf));
	if (copied < 0 || copied == sizeof(buf))
		return -EFAULT;
	printk(KERN_INFO "%s", buf);
	return 0;
} 
*/