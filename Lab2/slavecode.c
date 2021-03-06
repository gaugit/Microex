/* 
 * File:   pic_main.c
 * Author: rj
 *
 * Created on 17 September, 2014, 4:46 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <pic16f688.h>
#include "main.h"
/*
 * 
 */


#pragma config WDTE=OFF, MCLRE=OFF, FOSC=INTOSCIO
unsigned char slave_state = 1 ;
unsigned char master_cmd  = 0 ;
unsigned int adc_read = 0 ;
unsigned int time_out = 0 ;
unsigned char ReadOrProcess = 1;

int Readadc(void)
{
static unsigned char adc_state = 0;
static int adcval = 0;

    switch(adc_state)
    {
        case 0:
                ADCON0bits.GO_DONE = 1 ;
                adc_state = 1 ;
                break ;
        case 1 :
                if(ADCON0bits.GO_DONE == 0)
                {
                    adcval = ((ADRESH << 8) | ADRESL);
                    adc_state = 0 ;
                }
                else
                {
                  adc_state = 1 ;
                }
                break ;
    }
    return adcval;
}


int main(int argc, char** argv) {

unsigned int time_out = 0 ;
unsigned int adcval   = 0 ;
unsigned char value=0;


SYS_INIT();

while(1){

    adc_read = Readadc();

   slave_state_machine();
}
   return (EXIT_SUCCESS);
}



void slave_state_machine(void)
{
    
 switch(slave_state)
 {
     case POLL_STROBE :
            TRISC  = PORTC_IN;
            if(STROBE == RESET )
            {
              slave_state = POLL_STROBE ;
            }
            else
            {
             if(ReadOrProcess == 1)
              slave_state = READ_CMD ;
             else
              slave_state = PROCESS_CMD ;
            }

         break;

     case READ_CMD :
            

            if(STROBE == RESET )
            {
              ReadOrProcess = 0 ;
              slave_state = POLL_STROBE ;
            }
            else
            {
              master_cmd = (unsigned char)PORTC ;
              master_cmd = master_cmd & 0x0f;
              slave_state = READ_CMD ;
            }

          break;

     case PROCESS_CMD :

         slave_state = slave_response(master_cmd);

         break;

     default :
          slave_state = POLL_STROBE;
 }
}

/*****************************************************************************/
// RA2 = analog input
// RC0,RC1,RC2,RC3 are data pins
// RC4 is strobe signal
/*****************************************************************************/
void SYS_INIT(void)
{
unsigned int adc_read = 0;

OSCCON = 0b01110001; //8MHZ clock using internal oscillator
TRISA  = 0b00000100; // PORTA RA2 set as input for ADC
ANSEL  = 0b00000100; //set pin RA2 as analog for adc and AN4-7 (RC0-3) as digital for Galileo
ADCON1 = 0b0101000; //clock conversion FOSC/16
ADCON0 = 0b10001001; // ADC enable / Right justified / AN2 channel selected
CMCON0 = 7 ;
// init of portA pins

}

unsigned char slave_response(unsigned char command)
{
    unsigned char counter = 0;
    unsigned char array[4];
    unsigned char state;
    unsigned char prev_level;

    switch(command)
    {
        case MSG_RESET:
                        
                        SYS_INIT();
                        TRISC    = PORTC_OUT;
                        state = PROCESS_CMD ;
                        if(STROBE == SET)
                        {
                          PORTC = MSG_ACK & 0x0f;
                        }
                        else
                        {
                         state = POLL_STROBE;
                         ReadOrProcess = 1  ;
                        }
                        break;

        case MSG_PING:
                     //   PORTAbits.RA4 = 0;
                       TRISC    = PORTC_OUT;
                        state = PROCESS_CMD ;
                        if(STROBE == SET)
                        {
                          PORTC = MSG_ACK & 0x0f;
                        }
                        else
                        {
                         state = POLL_STROBE;
                         ReadOrProcess = 1  ;
                        }

                       break ;


        case MSG_GET:
                    TRISC    = PORTC_OUT;
                    array[0] = (unsigned char)(adc_read & 0x000f);
                    array[1] = (unsigned char)((adc_read & 0x00f0)>> 4);
                    array[2] = (unsigned char)((adc_read & 0x0f00)>>8);
                    array[3] = (unsigned char) MSG_ACK ;

                    PORTC = (array[counter] & 0x0f) ;
                    while(counter < 4)
                    {
                        if(STROBE == SET)
                        {
                        
                         prev_level = 1;
                        }
                        else if((prev_level == 1) && (STROBE == RESET))
                        {   counter++ ;
                            PORTC = (array[counter] & 0x0f) ;
                            prev_level = 0;
                            
                        }

                        time_out++;
                        if(time_out > TIME_TICKS)
                        {
                          time_out = 0 ;
                          break;
                        }
                    }
                    counter = 0;
                    state = POLL_STROBE;
                    ReadOrProcess = 1;
                        break;

    }

    return(state);
}







