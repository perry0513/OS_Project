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

int main (int argc, char* argv[])
{
	char buf[BUF_SIZE];
	int i, dev_fd, file_fd; // the fd for the device and the fd for the input file
	size_t ret, file_size = 0, data_size = -1, offset=0;
	char file_name[50];
	char method[20];
	char ip[20];
	struct timeval start;
	struct timeval end;
	double trans_time; //calulate the time between the device is opened and it is closed
	char *kernel_address, *file_address;


	strcpy(file_name, argv[1]);
	strcpy(method, argv[2]);
	strcpy(ip, argv[3]);

	// should be O_RDWR for PROT_WRITE when mmap()
	if( (dev_fd = open("/dev/slave_device", O_RDWR)) < 0)
	{
		perror("failed to open /dev/slave_device\n");
		return 1;
	}
	// time when the device is opened
	gettimeofday(&start ,NULL);
	if( (file_fd = open (file_name, O_RDWR | O_CREAT | O_TRUNC)) < 0)
	{
		perror("failed to open input file\n");
		return 1;
	}

	// 0x12345677 : connect to master in the device
	// 0x12345677 : master_IOCTL_CREATESOCK in master_device.c
	if(ioctl(dev_fd, 0x12345677, ip) == -1)
	{
		perror("ioclt create slave socket error\n");
		return 1;
	}

	// write "ioctl success\n" to stdout (fd = 1)
    write(1, "ioctl success\n", 14);

	switch(method[0])
	{
		case 'f': // fcntl : read()/write()
			do
			{
				// read from the device, ret is the number of bytes read
				ret = read(dev_fd, buf, sizeof(buf));
				// write the number of bytes read to the file
				write(file_fd, buf, ret);
				// increment file_size
				file_size += ret;
			} while(ret > 0);
			break;
		case 'm':
			while (1) {
				ret = ioctl(dev_fd, 0x12345678);
				if (ret == 0) {
					file_size = offset;
					break;
				}
				posix_fallocate(file_fd, offset, ret);
				file_address = mmap(NULL, ret, PROT_WRITE, MAP_SHARED, file_fd, offset);
				kernel_address = mmap(NULL, ret, PROT_READ, MAP_SHARED, dev_fd, offset);
				memcpy(file_address, kernel_address, ret);
				offset += ret;
			}
			break;
	}


	// end receiving data, close the connection
	// 0x12345679 : master_IOCTL_EXIT in master_device.c
	if(ioctl(dev_fd, 0x12345679) == -1)
	{
		perror("ioclt client exits error\n");
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


