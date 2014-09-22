/* 
 * File:   Lab1.c
 * Author: Gaurav Pitale
 *
 * Created on 12 September, 2014
 * Updated on 17 September, 2014
 */

#define Light_thres 700
#define LED_ON      PORTCbits.RC2 = 0
#define LED_OFF      PORTCbits.RC2 = 1
#include <pic16f688.h>

//config bits that are part-specific for the PIC16F688
#pragma config WDTE=OFF, MCLRE=OFF

/*
 * 
 */

void delay(unsigned dly)
{
 unsigned int temp;
 for(;dly>0;dly--)
 for(temp=1000;temp>0;temp--);
}

int Readadc(void)
{
    int adcval = 0;
    ADCON0bits.GO_DONE =  1;                //Start conversion
    while(ADCON0bits.GO_DONE == 1);        //waiting for ADC conversion to complete
    adcval = ((ADRESH << 8) | ADRESL);
    return adcval;
}


void main(void)
{
 unsigned int adc_read = 0;

 OSCCON = 0b01110001;              //8MHZ clock using internal oscillator
 TRISC = 0b00001000;              // PORTC RC3 set as input for ADC and RC2 set as output for LED

 ANSEL = 0b10000000;              //set pin RA7 as analog
 ADCON1 = 0b0101000;              //clock conversion FOSC/16
 ADCON0 = 0b10011101;             // ADC enable / Right justified / AD7 channel selected
 LED_OFF;
 while(1)
 {
  delay(10);
  adc_read = Readadc();             //Read LDR value
  if(adc_read < Light_thres)       //Checking LDR value
     LED_OFF;                     //RC2 is high level  //LED OFF
  else
     LED_ON;                      //RC2 is low level  //LED ON
 }

}
