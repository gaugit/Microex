
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rtc.h"
#include <semaphore.h>
#include <pthread.h>
#include "client.h"
#include "head.h"
#include <string.h>


#define IN 0
#define OUT 1
#define LOW 0
#define HIGH 1
/*****************************************************************************************/
/*****************************************************************************************/

void *UI_thread();
void *sense_thread();
void *client_thread();
/*****************************************************************************************/
#define STB 	26
#define GP0 	28
#define GP1	17
#define GP2	24
#define GP3	27

/* user commands */
#define MSG_RESET 0x0 	/* reset the sensor to initial state */
#define MSG_PING  0x1 	/* check if the sensor is working properly */
#define MSG_GET   0x2 	/* obtain the most recent ADC result */

/* Sensor Device Responses */
#define MSG_ACK     0xE /* acknowledgement to the commands */
#define MSG_NOTHING 0xF /* reserved */

/************************************************/
//CASE DECLERATIONS

#define SET_CLK         1
#define SET_DATE        2
#define READ_CLK        3
#define DATA_LOG	4
#define EXIT_APP        5
/************************************************/
//MUTEX DECLERATIONS
pthread_mutex_t switch_lock = PTHREAD_MUTEX_INITIALIZER ;

//MUTEX CONDITIONAL VARIABLE
pthread_cond_t cond;

//ADC_READ FLAGS AND DEFINITIONS

#define Yes	1
#define No	0

//Global variables
char timestamp[27];
int READ_ADC = 0;
int adc_data = 0;
int adc_flg  = 0;
int end_prog = 0; 
char ui_disp = 0;
rj http;
/***************************************************************************************8*/
int write_cmd(unsigned char cmd)
{
	unsigned char temp=0,i=0;
	int gpio_arr[4] = {GP0,GP1,GP2,GP3};

	if (-1 == GPIOWrite(STB, HIGH)) //Make strobe pin high
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

	if (-1 == GPIOWrite(STB, LOW)) //Make strobe pin low
	return(0);
}


int read_data(void)
{
	unsigned char data=0;

	if (-1 == GPIOWrite(STB, HIGH)) //Make strobe pin high
	return(1);

	if (-1 == GPIODirection(GP0, IN) || -1 == GPIODirection(GP1, IN) || -1 == GPIODirection(GP2, IN) || -1 == GPIODirection(GP3, IN))
	return(2);

	usleep(1000);

	data = (GPIORead(GP0) | (GPIORead(GP1) << 1) | (GPIORead(GP2) << 2) | (GPIORead(GP3) << 3));

	if (-1 == GPIOWrite(STB, LOW)) //Make strobe pin low
	return(3);

	return data;
}


int main(int argc, char *argv[])
{
	
	unsigned char in_ch=0,get_nib=0;
        pthread_t uit,sit,cit;
	char hostname[15],pass[10],grp[12],stat[10];
	
	http.host = &hostname[0];	
        http.password = &pass[0];
        http.name = &grp[0];
        http.status = &stat[0];

        strcpy(hostname,"129.63.205.75");
        http.port = 8000;
        http.id =2 ;
        strcpy(pass,"gkrd");
        strcpy(grp,"microex");
        strcpy(stat,"OK");
	init_rtc();

	if (-1 == GPIOExport(STB) || -1 == GPIOExport(GP0) || -1 == GPIOExport(GP1) || -1 == GPIOExport(GP2) || -1 == GPIOExport(GP3))
	return(1);
	/*
	* Set GPIO directions
	*/

	if (-1 == GPIODirection(STB, OUT))
	return(2);
	GPIOWrite(STB, LOW);
        
        printf("\n intializing processes .... ") ;    
            
        pthread_create(&uit,NULL,UI_thread,NULL); 
        pthread_create(&sit,NULL,sense_thread,NULL);
        pthread_create(&cit,NULL,client_thread,NULL); 
        
	pthread_join(uit,NULL);
	pthread_join(sit,NULL);
	pthread_join(cit,NULL);
           
	return(4);
}
     

void *UI_thread()
{
	static int sw_ch=0,log_time,tot_rec,counter;

	do
        {
	while(READ_ADC == Yes);	
	
	printf("\n /------------------------ LAB 4 ---------------------------------------/");
	printf("\n /-------------- WELCOME!! INTEL GALILEO BASED DAS ---------------------/");
	printf("\n /--------------- SELECT FROM FOLLOWING OPTIONS ------------------------/");
	printf("\n 1.SET RTC TIME\n 2.SET RTC DATE\n 3.READ RTC TIME\n 4.READ ADC \n 5.EXIT APPLICATION\n\n   ENTER OPTION : \t");
         
        READ_ADC = Yes;
  
	scanf("%d",&sw_ch);
     
       	switch(sw_ch)
	{

	  case SET_CLK :
		        set_rtc();
                        READ_ADC = No;
		        break;
	  case SET_DATE :
		        set_date();
		        READ_ADC = No ;
                        break;
	  case READ_CLK :
                        ui_disp = 1; 
		        read_rtc();
		        READ_ADC = No;
                        break;

          case DATA_LOG :		 	
			//pass signal to thread2 to update the adc value 					
         
                        ui_disp = 1;
                 	read_rtc();
			printf("\tLDR value = %d\n",adc_data);
			READ_ADC = No;
			
			break;

	  case EXIT_APP :
		         printf("\n GOODBYE !!! APPLICATION CLOSED\n ");
			 exit_rtc();
                         end_prog = 1;
		         break;
           default :  
                         printf("OOPS !!! WRONG OPTION ");
	}
        }while(end_prog == 0);

        pthread_exit(NULL);
}


void *sense_thread()
{
	do
	{	
		pthread_mutex_lock(&switch_lock);

		usleep(1000000);		
	
		if(write_cmd(MSG_GET) != 0 )
                printf("\nwrite failed");
                
		adc_data = (read_data() | (read_data() << 4) | (read_data() << 8));
		http.adcval = adc_data ; 
	        read_rtc();
		pthread_mutex_unlock(&switch_lock);	

	}while(end_prog == 0);
        pthread_exit(NULL);  
          
}

void *client_thread(void)
{
 do
 {
  client();
  sleep(2); 
  }while(end_prog == 0);
  pthread_exit(NULL); 
}




