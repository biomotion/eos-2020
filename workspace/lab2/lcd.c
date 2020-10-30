#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(int argc, char** argv){
    lcd_write_info_t display;
    lcd_full_image_info_t image;
    int fd, ret;
    unsigned int i;
    printf("start\n");
    if((fd=open("/dev/lcd", O_RDWR)) < 0){
        printf("OPEN /dev/lcd FAILED.\n");
        exit(-1);
    }

    ret = ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    display.Count = sprintf((char*) display.Msg, "HELLO WORLD\n");
    ret = ioctl(fd, LCD_IOCTL_WRITE, &display);
    sleep(1);

    // ret = ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    // printf("%d\n", sizeof(unsigned int));
    // sleep(1);
    // for(i=0; i<0x800; i++){
    //     if(i%16 == i/(16*8)){
    //         image.data[i] = 0x01 << ((i/8)%16);
    //         printf("%d\n", 0x01 << ((i/8)%16));
    //     }else if(i%16 == i/(16*8)){

    //     }else{
    //         image.data[i] = 0;
    //     }
        
    // }
    // ret = ioctl(fd, LCD_IOCTL_DRAW_FULL_IMAGE, &image);
    close(fd);

    return 0;

}
