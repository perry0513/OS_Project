# 2019 Fall OS Project1: Scheduler
* Installing
	* Download Linux-4.17.4<br>
	`wget https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.17.4.tar.xz`
	* Extract kernel source code<br>
	`sudo tar Jxvf linux-4.17.4.tar.xz -C/usr/src/`	
	* Copy the files in `kernel_files` to `/usr/src/linux-4.17.4/kernel`<br>
	`sudo cp kernel_files/* /usr/src/linux-4.17.4/kernel`
	* Please refer to this slide for the following steps: https://docs.google.com/presentation/d/1KwS9PuGZxLXQ9IEYJE7KUhnz3bHohJLWOH-CRGZkC40/edit#slide=id.p28
	* Compile kernel and reboot, choose Linux-4.17.4

* Compiling
	* `make` in current directory
	* To play with the program, type `sudo ./main < test_file`
	* To see the results, type `dmesg | grep project1`
