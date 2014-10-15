/*
 * File:   Lab2_linux.c
 * Author: Gaurav Pitale
 *
 * Created on 5 October, 2014
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1


static int GPIOExport(int pin)
{
#define BUFFER_MAX 3
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

static int GPIOUnexport(int pin)
{
	char buffer[BUFFER_MAX];
	ssize_t bytes_written;
	int fd;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}

	bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}

static int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
	char path[DIRECTION_MAX];
	int fd;

	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}

	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}

	close(fd);
	return(0);
}

static int GPIORead(int pin)
{
#define VALUE_MAX 30
	char path[VALUE_MAX];
	char value_str[3];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return(-1);
	}

	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}

	close(fd);

	return(atoi(value_str));
}

static int GPIOWrite(int pin, int value)
{
	static const char s_values_str[] = "01";

	char path[VALUE_MAX];
	int fd;

	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return(-1);
	}

	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		return(-1);
	}

	close(fd);
	return(0);
}

#define STB            26
#define GP0            28
#define GP1	       17
#define GP2	       24
#define GP3	       27


/* user commands */
#define MSG_RESET 0x0 /* reset the sensor to initial state */
#define MSG_PING  0x1 /* check if the sensor is working properly */
#define MSG_GET   0x2 /* obtain the most recent ADC result */

/* Sensor Device Responses */
#define MSG_ACK 0xE   /* acknowledgement to the commands */
#define MSG_NOTHING 0xF /* reserved */

int write_cmd(unsigned char cmd)
{
    unsigned char temp=0,i=0;
    int gpio_arr[4] = {GP0,GP1,GP2,GP3};

    if (-1 == GPIOWrite(STB, HIGH))          //Make strobe pin high
	return(3);

    if (-1 == GPIODirection(GP0, OUT) || -1 == GPIODirection(GP1, OUT) || -1 == GPIODirection(GP2, OUT) || -1 == GPIODirection(GP3, OUT))
	return(2);

	temp = cmd;
	for(i=0;i<4;i++)
        {
            if((temp & 0x01) == 1)
            {
                GPIOWrite(gpio_arr[i], HIGH);
            }
            else
            {
               GPIOWrite(gpio_arr[i], LOW);
            }

            temp = temp >> 1;

        }

        usleep(10000);
        if (-1 == GPIOWrite(STB, LOW))          //Make strobe pin low
        return(0);
}

int read_data(void)
{
    unsigned char data=0;
    if (-1 == GPIOWrite(STB, HIGH))          //Make strobe pin high
	return(1);

    if (-1 == GPIODirection(GP0, IN) || -1 == GPIODirection(GP1, IN) || -1 == GPIODirection(GP2, IN) || -1 == GPIODirection(GP3, IN))
	return(2);
    usleep(10000);
    data = (GPIORead(GP0) | (GPIORead(GP1) << 1) | (GPIORead(GP2) << 2) | (GPIORead(GP3) << 3));
    
    if (-1 == GPIOWrite(STB, LOW))          //Make strobe pin low
	return(3);

    return data;
}

int main(int argc, char *argv[])
{
    int adc_data=0,sw_ch=0;
    unsigned char in_ch=0,get_nib=0;
    if (-1 == GPIOExport(STB) || -1 == GPIOExport(GP0) || -1 == GPIOExport(GP1) || -1 == GPIOExport(GP2) || -1 == GPIOExport(GP3))
	return(1);

	/*
	 * Set GPIO directions
	 */
    if (-1 == GPIODirection(STB, OUT))
	return(2);
	
    GPIOWrite(STB, LOW);

    while(1)
    {
		printf("Enter the choice\n1:Send Ping\n2:Send Reset\n3:Get data\n4:Exit\n");
		scanf("%d",&sw_ch);
		switch(sw_ch)
		{
		case 1:
			if(write_cmd(MSG_PING) != 0)
		           printf("Write command failed\n");
			get_nib = read_data();
                        if(get_nib == 0xE)
			   printf("ACK received\n");
			else
			   printf("Ping command failed\n");
			break;
		case 2:
			if(write_cmd(MSG_RESET) != 0)
		           printf("Write command failed\n");
			get_nib = read_data();
			
			if(get_nib == 0xE)
			   printf("ACK received\n");
			else
			   printf("Reset command failed\n");
			break;
		case 3:
			if(write_cmd(MSG_GET) != 0)
		           printf("Write command failed\n");
		           
		        adc_data = (read_data() | (read_data() << 4) | (read_data() << 8));
                        printf("LDR value = %d\n",adc_data);
			break;
		case 4:
                        in_ch = 1;
                        break;
                }
                if(in_ch == 1)
                {
                 in_ch=0; break;
                }
	}
	
    if (-1 == GPIOUnexport(STB) || -1 == GPIOUnexport(GP0) || -1 == GPIOUnexport(GP1) || -1 == GPIOUnexport(GP2) || -1 == GPIOUnexport(GP3))
        return(4);
}

