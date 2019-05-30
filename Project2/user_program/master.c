#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define PAGE_SIZE 4096
#define BUF_SIZE 512
size_t get_filesize(const char* filename); // get the size of the input file


int main (int argc, char* argv[])
{
	char buf[BUF_SIZE];
	int i, dev_fd, file_fd; // the fd for the device and the fd for the input file
	size_t ret, file_size, offset = 0, tmp;
	char file_name[50], method[20];
	char *kernel_address = NULL, *file_address = NULL;
	struct timeval start;
	struct timeval end;
	double trans_time; // calulate the time between the device is opened and it is closed


	strcpy(file_name, argv[1]);
	strcpy(method, argv[2]);

	// open device
	if( (dev_fd = open("/dev/master_device", O_RDWR)) < 0)
	{
		perror("failed to open /dev/master_device\n");
		return 1;
	}
	// time when the device is opened
	gettimeofday(&start ,NULL);
	// open file
	if( (file_fd = open (file_name, O_RDWR)) < 0 )
	{
		perror("failed to open input file\n");
		return 1;
	}

	// get file size
	if( (file_size = get_filesize(file_name)) < 0)
	{
		perror("failed to get filesize\n");
		return 1;
	}

	// 0x12345677 : create socket and accept the connection from the slave
	// 0x12345677 : master_IOCTL_CREATESOCK in master_device.c
	if(ioctl(dev_fd, 0x12345677) == -1) 
	{
		perror("ioclt server create socket error\n");
		return 1;
	}


	switch(method[0])
	{
		case 'f': // fcntl : read()/write()
			do
			{
				// read from the input file, ret is the number of bytes read
				ret = read(file_fd, buf, sizeof(buf));
				// write the number of bytes read to the the device
				write(dev_fd, buf, ret);
			} while(ret > 0);
			break;
	}

	// end sending data, close the connection
	// 0x12345679 : master_IOCTL_EXIT in master_device.c
	if(ioctl(dev_fd, 0x12345679) == -1)
	{
		perror("ioclt server exits error\n");
		return 1;
	}
	// time when device is closed
	gettimeofday(&end, NULL);

	// calculate time 
	trans_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)*0.0001;
	printf("Transmission time: %lf ms, File size: %d bytes\n", trans_time, file_size / 8);

	close(file_fd);
	close(dev_fd);

	return 0;
}

size_t get_filesize(const char* filename)
{
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}
