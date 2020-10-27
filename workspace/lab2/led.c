/*
* led.c -- the sample code for controlling LEDs on Creator .
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(int argc, char** argv){
    int fd;
    int retval;

    unsigned short data;
    // Open Device /dev/lcd
    if((fd=open("/dev/lcd", O_RDWR)) < 0){
        printf("OPEN /dev/lcd FAILED.\n");
        exit(-1);
    }

    // Turn on all LED lamps
    data = LED_ALL_ON;
    retval = ioctl(fd, LED_IOCTL_SET, &data);
    printf("Turn on all LED lamps...(%d)\n", retval);
    sleep(3);

    // Turn off all LED lamps
    data = LED_ALL_OFF;
    retval = ioctl(fd, LED_IOCTL_SET, &data);
    printf("Turn off all LED lamps(%d)\n", retval);
    sleep(3);

    // Turn on D9
    data = LED_D9_INDEX;
    retval = ioctl(fd, LED_IOCTL_BIT_SET, &data);
    printf("Turn on D9(%d)\n", retval);
    sleep(3);

    // Turn off D9
    data = LED_D9_INDEX;
    retval = ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
    printf("Turn off D9(%d)\n", retval);
    sleep(3);

    close(fd);

    return 0;

}
