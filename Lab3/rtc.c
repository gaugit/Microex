#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "gpio.h"
#inculde "rtc.h"

unsigned char conv_dec(unsigned char hexnumber)
{
 return ((hexnumber & 0x0f) + (((hexnumber & 0xf0) >> 4)*10));
}

int Init_rtc(void)
{
 int fd;
 int r ;
  unsigned int console_cmd ;
  char *dev = "/dev/i2c-0";
  unsigned char cmd[4]={0,12,12,12};
  
  if (-1 == GPIOExport(I2C_EN) )
	return(1);

  if (-1 == GPIODirection(I2C_EN, OUT))
  return(2);
  GPIOWrite(I2C_EN, LOW);

  fd = open(dev, O_RDWR );
  if(fd < 0)
  {
     perror("Opening i2c device node\n");
     return 1;
  }
  usleep(2);
  r=ioctl(fd,I2C_SLAVE,dev_addr);


usleep(2);
    if(r<0)
    {
      printf("\n ioctl failed");
    }

  r=write(fd,&cmd,4);
    if(r==4)
    {
      printf("write error");
    }
  printf("RTC init success\n");  
  return fd;
}

int set_rtc(int fd)
{

    unsigned char new_time[9];
    unsigned char hhmmss[5];
    unsigned char command[9]={0x00,SEC,MIN,HR,DAY,DATE,MONTH,YEAR};
    int r;
/*
    printf("\n Enter the new time in hr:min:sec format = ");
    scanf("%s",&new_time);

    hhmmss[0] = 0 ;
    hhmmss[3] = (unsigned char)(((new_time[0]-0x30)<< 4)|(new_time[1]-0x30));
    hhmmss[2] = (unsigned char)(((new_time[3]-0x30)<< 4)|(new_time[4]-0x30));
    hhmmss[1] = (unsigned char)(((new_time[6]-0x30)<< 4)|(new_time[7]-0x30));

    printf("Hr:%d, Min:%d, sec:%d\n",con_dec(hhmmss[3]),con_dec(hhmmss[2]), con_dev(hhmmss[1]));
*/

    r = write(fd, &command, 8);
    usleep(delay);
    if(r < 0)
    {
       perror("write failed RTC values \n");
       return 0;
    }
    else
    {
       printf("\n Stored new time");

    }
}

int read_rtc(int fd)
{
    unsigned char read_data[10];
    unsigned char temp[18];
    int r;

    printf("\nhello i am read");
//    r = ioctl(fd, I2C_SLAVE, dev_addr);
    read_data[0] = 0;
/*
    if(r < 0)
    {
       perror("Selecting i2c device\n");
       printf("\n ioctl error");
	return 0;
    }
*/
    r = write(fd, &read_data[0], 1);
    usleep(delay);
    if(r != 1)
    {
       perror("write failed \n");
       return 0;
    }

    r = read(fd, &read_data[1], 7);

printf("T %02x:%02x:%02x D: %02x:%02x:%02x\n",read_data[3],read_data[2],read_data[1],read_data[6],read_data[5],read_data[7]);

printf("T %02d:%02d:%02d D: %02d:%02d:%02d",conv_dec(read_data[3]),conv_dec(read_data[2]),conv_dec(read_data[1]),conv_dec(read_data[6]),conv_dec(read_data[5]),conv_dec(read_data[7]));
}
