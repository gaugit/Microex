/* 
 * File:   main.h
 * Author: rj
 *
 * Created on 20 September, 2014, 7:43 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif

void PIC_ADC_INIT(void);
void SYS_INIT(void);
void slave_state_machine(void);
unsigned char slave_response(unsigned char command);

#define ANALOG          1
#define DIGITAL         0

#define RIGHT_JUSTIFIED 1
#define LEFT_JUSTIFIED  0

#define VREF_PIN        1
#define VDD             0

#define START_ADC       1
#define ADC_RDY        0

#define CH_0            0
#define CH_1            1
#define CH_2            2
#define CH_3            3
#define CH_4            4
#define CH_5            5
#define CH_6            6
#define CH_7            7



#define ENABLE_ADC      1
#define DISABLE_ADC     0


// state definitions of the state machine
#define POLL_STROBE     1
#define READ_CMD        2
#define PROCESS_CMD     3


#define TIME_TICKS      0xfffe
#define THRESHOLD       500

// porta configuration as input and output
#define PORTC_IN        0b00011111
#define PORTC_OUT       0b00010000

#define SET             1
#define RESET           0

#define STROBE          PORTCbits.RC4

//Command definitions from received from master
#define MSG_RESET 0x0 /* reset the sensor to initial state */
#define MSG_PING  0x1 /* check if the sensor is working properly */
#define MSG_GET   0x2 /* obtain the most recent ADC result */

/* Sensor Device Responses */
#define MSG_ACK     0x0E /* acknowledgement to the commands */
#define MSG_NOTHING 0xF /* reserved */


#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

