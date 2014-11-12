#ifndef PIC_READ_H

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int Init_Pic_mod(void);
void Reset_pic(void);
int Read_pic(void);

#endif // PIC_READ_H
