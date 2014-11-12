#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "gpio.h"
#inculde "rtc.h"
#include "pic_read.h"

#define SET_TIME    1   //set 1 to set time values in RTC chip

void main(void)
{
 int rtc_fd=0,ldr_val=0;
 Init_Pic_mod();
 rtc_fd = Init_rtc();
 
 if(SET_TIME == 1)
 {
  set_rtc(rtc_fd);
 }
 printf("Starting Sensor log\n");
 
 while(1)
 {
  read_rtc(rtc_fd);
  ldr_val = Read_pic();
  printf("LDR Value = %d",ldr_val);
  sleep(5);                          //5sec wait time
 }
}
