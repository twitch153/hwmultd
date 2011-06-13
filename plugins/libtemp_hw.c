
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include <hwcommon.h>


void
hwplugin_init()
{
	return ;
}

void
hwplugin_fini()
{
	return ;
}




int fd ;

#define DELAY 15000

int
init_hw()
{
	unsigned char data[1024];
	const char *dev = "/dev/ttyUSB0" ;
	struct termios ios;

	if((fd = open( dev, O_RDWR | O_NONBLOCK | O_NOCTTY )) < 0)
		return -1;

	if(tcgetattr(fd, &ios) < 0)
	{
		close(fd);
		return -2;
	}

	cfsetispeed(&ios, B9600);
	cfsetospeed(&ios, B9600);
	cfmakeraw  (&ios);
	if(tcsetattr(fd, TCSANOW, &ios) < 0)
	{
		close(fd);
		return -3;
	}

	usleep(DELAY);
	if(write(fd, "X", 1) < 1)
		return -4;

	usleep(DELAY);		// Don't check for error on read since
	read(fd, data, 1024);	// errno=EAGAIN may result from O_NONBLOCK

	usleep(DELAY);
	if(write(fd, "P", 1) < 1)
		return -6;

	usleep(DELAY);
	read(fd, data, 1024);

	usleep(DELAY);
	if(write(fd, "R", 1) < 1)
		return -6;

	usleep(DELAY);
	read(fd, data, 1024);

	return 1;
}

int
reset_hw()
{
	usleep(DELAY);
	if(close_hw() != 1)
		return -1;

	usleep(DELAY);
	if(init_hw() != 1)
		return -2;

	return 1;
}

char *
read_hw()
{
	int i;
	unsigned char data[18];
	double temp;
	char *stemp;

	usleep(DELAY);
	if(write(fd, "R", 1) < 1)
		return "NULL WRITE";

	usleep(DELAY);
	if(read(fd, data, 18) < 18)
		return "NULL READ";

	for(i = 0; i < 18; i++)
		if(data[i])
			break;

	temp = data[i] + 256.0 * data[i+1];
	temp /= 16.0;

	stemp = (char *)malloc(1024);
	sprintf(stemp, "%lf", temp);

	/*
	char *stemp = (char *)malloc(1024);
	sprintf(stemp, "%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %lf",
		data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
		data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15],
		data[16], data[17], temp);
	*/

	return stemp;
}

int
close_hw()
{
	if(close(fd))
		return -1;
	else
		return 1;
}
