#ifndef RTC_H

#define SEC   0x00
#define MIN   0x00
#define HR    0x15
#define DAY   0x03
#define DATE  0x12
#define MONTH 0x11
#define YEAR  0x14

int Init_rtc(void);
int set_rtc(int fd);
int read_rtc(int fd);


#endif  //RTC_H
