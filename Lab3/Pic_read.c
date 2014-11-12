#include "gpio.h"

#define STB            26
#define GP0            28
#define GP1	           17
#define GP2	           24
#define GP3	           27


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

void Reset_pic(void)
{
 unsigned char get_nib=0;
 if(write_cmd(MSG_RESET) != 0)
		           printf("Write command failed\n");
			get_nib = read_data();
			
			if(get_nib == MSG_ACK)
			   printf("ACK received\n");
			else
			   printf("Reset command failed\n");
			
			get_nib = 0; 
}

int Read_pic(void)
{
 int adc_data=0;
 if(write_cmd(MSG_GET) != 0)
 printf("Write command failed\n");
		           
  adc_data = (read_data() | (read_data() << 4) | (read_data() << 8));
  //printf("LDR value = %d\n",adc_data);
  return adc_data;  
}

int Init_Pic_mod(void)
{
 if (-1 == GPIOExport(STB) || -1 == GPIOExport(GP0) || -1 == GPIOExport(GP1) || -1 == GPIOExport(GP2) || -1 == GPIOExport(GP3))
	return(1);

	/*
	 * Set GPIO directions
	 */
  if (-1 == GPIODirection(STB, OUT))
	return(2);
	
  GPIOWrite(STB, LOW);
  Reset_pic();
}
