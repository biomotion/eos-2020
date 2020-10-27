#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(int argc, char** argv){
    _7seg_info_t data;
    int fd, ret, i;

    if((fd=open("/dev/lcd", O_RDWR)) < 0){
        printf("OPEN /dev/lcd FAILED.\n");
        exit(-1);
    }

    printf("Turning on 7 segments\n");
    ioctl(fd, _7SEG_IOCTL_ON, NULL);
    data.Mode = _7SEG_MODE_HEX_VALUE;
    data.Which = _7SEG_ALL;
    data.Value = 0x2004;
    ioctl(fd, _7SEG_IOCTL_SET, &data);
    sleep(3);

    printf("Changing to 5008\n");
    data.Mode = _7SEG_MODE_PATTERN;
    data.Which = _7SEG_D5_INDEX | _7SEG_D8_INDEX;
    data.Value = 0x6d7f; // change to 5008
    ioctl(fd, _7SEG_IOCTL_SET, &data);
    sleep(3);

    printf("Done\n");
    ioctl(fd, _7SEG_IOCTL_OFF, NULL);
    close(fd);

    return 0;

}
