#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(int argc, char** argv){
    unsigned short key;
    int fd, ret;
    printf("start\n");
    if((fd=open("/dev/lcd", O_RDWR)) < 0){
        printf("OPEN /dev/lcd FAILED.\n");
        exit(-1);
    }

    ioctl(fd, KEY_IOCTL_CLEAR, key);
    while(1){
        ret = ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key);

        // ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
        // if(ret < 0){
        //     printf("n\n");
        //     sleep(1);
        //     continue;
        // }else{
        //     printf("y\n");
        // }
        // ret = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
        printf("%c\n", (key & 0xff));
    }
    close(fd);

    return 0;

}
