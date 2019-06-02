#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/net.h>
#include <net/sock.h>
#include <asm/processor.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/mm.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#ifndef VM_RESERVED
#define VM_RESERVED   (VM_DONTEXPAND | VM_DONTDUMP)
#endif

#define DEFAULT_PORT 2325
#define master_IOCTL_CREATESOCK 0x12345677
#define master_IOCTL_MMAP 0x12345678
#define master_IOCTL_EXIT 0x12345679
#define BUF_SIZE 512

typedef struct socket * ksocket_t;

struct dentry  *file1;//debug file

//functions about kscoket are exported, and thus we use extern here
extern ksocket_t ksocket(int domain, int type, int protocol);
extern int kbind(ksocket_t socket, struct sockaddr *address, int address_len);
extern int klisten(ksocket_t socket, int backlog);
extern ksocket_t kaccept(ksocket_t socket, struct sockaddr *address, int *address_len);
extern ssize_t ksend(ksocket_t socket, const void *buffer, size_t length, int flags);
extern int kclose(ksocket_t socket);
extern char *inet_ntoa(struct in_addr *in);//DO NOT forget to kfree the return pointer

static int __init master_init(void);
static void __exit master_exit(void);

int master_close(struct inode *inode, struct file *filp);
int master_open(struct inode *inode, struct file *filp);
static long master_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
static ssize_t send_msg(struct file *file, const char __user *buf, size_t count, loff_t *data);//use when user is writing to this device

static ksocket_t sockfd_srv, sockfd_cli;//socket for master and socket for slave
static struct sockaddr_in addr_srv;//address for master
static struct sockaddr_in addr_cli;//address for slave
static mm_segment_t old_fs;
static int addr_len;
//static  struct mmap_info *mmap_msg; // pointer to the mapped data in this device

//file operations
static struct file_operations master_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = master_ioctl, //149行
	.open = master_open, //143行 目前只有回傳0
	.write = send_msg,
	.release = master_close //138行 目前只有回傳0
};
//：）
//device info
//miscdevice 是一個小型的驅動程式
//直接參考這篇！！：http://nano-chicken.blogspot.com/2009/12/linux-modules6-miscdev.html
static struct miscdevice master_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "master_device",
	.fops = &master_fops
};

static int __init master_init(void) //主要執行這個function
{
	int ret;
	file1 = debugfs_create_file("master_debug", 0644, NULL, NULL, &master_fops);

	//register the device
	if( (ret = misc_register(&master_dev)) < 0){
		printk(KERN_ERR "misc_register failed!\n");
		return ret;
	}

	printk(KERN_INFO "master has been registered!\n");

	//這篇解釋得很清楚：https://www.cnblogs.com/bittorrent/p/3264211.html
	//簡單來說就是 把系統函式所能存取的記憶體空間重新設定為0—4GB
	old_fs = get_fs(); //type mm_segment_t
	set_fs(KERNEL_DS);

	//initialize the master server
	sockfd_srv = sockfd_cli = NULL;
	memset(&addr_cli, 0, sizeof(addr_cli)); //用0把前面那個全部bit設成0
	memset(&addr_srv, 0, sizeof(addr_srv));
	addr_srv.sin_family = AF_INET;
	addr_srv.sin_port = htons(DEFAULT_PORT); //利用hton得到Network Byte Order
	addr_srv.sin_addr.s_addr = INADDR_ANY;
	//上面這幾行都是固定的，設定好一個詳細的address及通信方式
	addr_len = sizeof(struct sockaddr_in);

	sockfd_srv = ksocket(AF_INET, SOCK_STREAM, 0);
	printk("sockfd_srv = 0x%p  socket is created \n", sockfd_srv);
	if (sockfd_srv == NULL)
	{
		printk("socket failed\n");
		return -1;
	}
	if (kbind(sockfd_srv, (struct sockaddr *)&addr_srv, addr_len) < 0)
	{
		printk("bind failed\n");
		return -1;
	}
	if (klisten(sockfd_srv, 10) < 0)
	{
		printk("listen failed\n");
		return -1;
	}
    printk("master_device init OK\n");
	set_fs(old_fs);//回覆剛剛上面 把系統函式所能存取的記憶體空間重新設定為0—4GB 那步
	return 0;
}

static void __exit master_exit(void)
{
	misc_deregister(&master_dev);
    printk("misc_deregister\n");
	if(kclose(sockfd_srv) == -1)
	{
		printk("kclose srv error\n");
		return ;
	}
	set_fs(old_fs);
	printk(KERN_INFO "master exited!\n");
	debugfs_remove(file1);
}

int master_close(struct inode *inode, struct file *filp)
{
	return 0;
}

int master_open(struct inode *inode, struct file *filp)
{
	return 0;
}


static long master_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	long ret = -EINVAL;
	size_t data_size = 0, offset = 0;
	char *tmp;
	
	//每個pgd條目指向一個pud，以此類推
	//pgd->pud->pmd->pte->page physical address
	//(詳情參閱第八章 或 https://blog.csdn.net/yrj/article/details/2508785

	//先在terminl跑一下uname -m
	//如果是32位元，會跑出i開頭的
	//如果是64位元，會跑出x86_64
	
	//基本上除非用PAE不然只要看pgd, pte就好

	

	pgd_t *pgd; //Page Global Directory (PGD)
	p4d_t *p4d;
	pud_t *pud; //Page Upper Directory (PUD)
	pmd_t *pmd; //Page Middle Directory (PMD)
    pte_t *ptep, pte; //Page Table (PTE)
	old_fs = get_fs(); //type mm_segment_t
	set_fs(KERNEL_DS);
	switch(ioctl_num)
	{
		case master_IOCTL_CREATESOCK:// create socket and accept a connection
			sockfd_cli = kaccept(sockfd_srv, (struct sockaddr *)&addr_cli, &addr_len);
			if (sockfd_cli == NULL)
			{
				printk("accept failed\n");
				return -1;
			}
			else
				printk("aceept sockfd_cli = 0x%p\n", sockfd_cli);

			tmp = inet_ntoa(&addr_cli.sin_addr);
			printk("got connected from : %s %d\n", tmp, ntohs(addr_cli.sin_port));
			kfree(tmp);
			ret = 0;
			break;
		case master_IOCTL_MMAP:
			break;
		case master_IOCTL_EXIT:
			if(kclose(sockfd_cli) == -1)
			{
				printk("kclose cli error\n");
				return -1;
			}
			ret = 0;
			break;
		default: //印出他的physical address
			pgd = pgd_offset(current->mm, ioctl_param);
			p4d = p4d_offset(pgd, ioctl_param);
			pud = pud_offset(p4d, ioctl_param);
			pmd = pmd_offset(pud, ioctl_param);
			ptep = pte_offset_kernel(pmd , ioctl_param);
			pte = *ptep;
			printk("master: %lX\n", pte);
			ret = 0;
			break;
	}

	set_fs(old_fs);
	return ret;
}
static ssize_t send_msg(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
//call when user is writing to this device
	char msg[BUF_SIZE];
	/*
	static inline unsigned long copy_from_user(void *to, const void __user *from, unsigned long n)
	*to (kernel space 的pointer)
	*from (user space 的pointer)
	n (user space 想從 kernel space copy 的字數)
	return 沒有拷貝成功的字數
	=> 拷貝成功就會回傳0
	*/
	if(copy_from_user(msg, buf, count)) //if 有字數沒有從user space copy成功
		return -ENOMEM;
	ksend(sockfd_cli, msg, count, 0); //如果都copy成功的話就送出


	return count;

}




module_init(master_init);
module_exit(master_exit);
MODULE_LICENSE("GPL");
