#ifndef _GPIO_H_
#define _GPIO_H_


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "gpio.h"
#include "head.h"
/*************************************************/
// prototype declaration
int init_rtc(void);
int set_rtc();
int read_rtc();
int set_date();
void exit_rtc(void);

void log_wait(int);
/***********************************************/
#define dev_addr        0x68
#define I2C_EN          29
#define delay           2

/**********************************************/
extern char timestamp[27];
extern char ui_disp;
/*********************************************/

 int fd ;

unsigned char conv_dec(unsigned char hexnumber)
{
 return ((hexnumber & 0x0f) + (((hexnumber & 0x0f) >> 4)*10));
}


int init_rtc(void)
{

 int r ;
 char *dev = "/dev/i2c-0";

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
      return 0;
}


int set_rtc()
{

    unsigned char new_time[9];
    unsigned char hhmmss[5];
    unsigned char command[9]={0x00,0x00,0x57,0x23,0x00,0x10,0x11,0x14}; 
    int r;

    printf("\n Enter the new time in hr:min:sec format = ");
    scanf("%s",&new_time);
    
    hhmmss[0] = 0 ;
    hhmmss[3] = (((new_time[0]-0x30) * 0x10)+(new_time[1]-0x30));
    hhmmss[2] = (((new_time[3]-0x30) * 0x10)+(new_time[4]-0x30));
    hhmmss[1] = (((new_time[6]-0x30) * 0x10)+(new_time[7]-0x30));

    r = write(fd, &hhmmss[0],4);
   
    usleep(delay);  
   
    if(r < 0)
    {
       perror("write failed rj \n");
       return 0;
    }
    else
    {
       printf("\n\t ************  NEW TIME UPDATE SUCCESSFUL **********");

    }

}


int set_date()
{

    unsigned char new_date[9];
    unsigned char mmddyy[5];
    unsigned char temp;
    int r;	

    printf("\n Enter the new time in mm:dd:yy format = ");
    scanf("%s",&new_date);

    mmddyy[0] = 4 ;
    mmddyy[2] = (unsigned char)(((new_date[0]-0x30) * 0x10)|(new_date[1]-0x30));
    mmddyy[1] = (unsigned char)(((new_date[3]-0x30) * 0x10)|(new_date[4]-0x30));
    mmddyy[3] = (unsigned char)(((new_date[6]-0x30) * 0x10)|(new_date[7]-0x30));

    r = write(fd, &mmddyy, 4);
    usleep(delay);

    if(r != 4)
    {
       perror("write failed \n");
       return 0;
    }
    else
    {
     printf("\n Stored new date");
    }


}


int read_rtc()
{
    char read_data[10];
    char temp[18];
    char temp_str;
    int r;
    read_data[0] = 0;

    r = write(fd, &read_data[0], 1);
//    usleep(delay); 

    if(r != 1)
    {
       perror("write failed \n");
       return 0;
    }

    r = read(fd, &read_data[1], 7);
	
    if(ui_disp == 1)
    { 	
     printf("\nDATE: %x:%x:%x\tTIME: %x:%x:%02x",read_data[6],read_data[5],read_data[7],read_data[3],read_data[2],read_data[1]);
     ui_disp = 0 ;
    }
    sprintf(timestamp,"20%x:%02x:%02x-%02x:%02x:%02x",read_data[7],read_data[6],read_data[5],read_data[3],read_data[2],read_data[1]);
    
}

void log_wait(int interval)
{
 int r,counter;
 unsigned char seconds,new_sec,addr=0;

 
 r = write(fd,&addr,1);
 
 r = read(fd,seconds,1);
 new_sec = seconds ;

 for(counter=0;counter<= interval;counter++)
 {
   while(new_sec == seconds)
   {
     write(fd,&addr,1);
     read(fd,&new_sec,1);

   }
   seconds = new_sec ;

 }

}


void exit_rtc(void)
{
    close(fd);
    GPIOUnexport(I2C_EN);	
}
 
#endif
