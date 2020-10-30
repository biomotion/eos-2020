#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

char interpret(unsigned short);
int calc_recursive(char* operations);
int segment_display_decimal(int fd, int data);
int led_display_binary(int fd, int data);

int main(int argc, char** argv){
  int fd, ret, i=0, result;
  unsigned short key;
  char expr[100] = {0}, op;
  lcd_write_info_t display;

  printf("start\n");
  if((fd=open("/dev/lcd", O_RDWR)) < 0){
    printf("OPEN /dev/lcd FAILED.\n");
    exit(-1);
  }

  
  ret = ioctl(fd, _7SEG_IOCTL_ON, NULL);
  ret = ioctl(fd, LCD_IOCTL_CLEAR, NULL);
  display.Count = sprintf((char*) display.Msg, ">");
  ret = ioctl(fd, LCD_IOCTL_WRITE, &display);
  ioctl(fd, KEY_IOCTL_CLEAR, key);
  while(1){
    ret = ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key);
    op = interpret(key);
    printf("%c\n", op);
    if(op == '#'){
      result = calc_recursive(expr);
      printf("%s = %d\n", expr, result);
      segment_display_decimal(fd, result);
      led_display_binary(fd, result);
      for(;i>=0; i--) expr[i] = '\0';
    }
    else if((key&0xff) == '*'){
      for(; i>=0; i--){
        expr[i] = '\0';
        ret = ioctl(fd, LCD_IOCTL_CLEAR, NULL);
      }
    }else{
      expr[i++] = op;
      printf("%s\n", expr);
      ret = ioctl(fd, LCD_IOCTL_CLEAR, NULL);
      display.Count = sprintf((char*) display.Msg, ">%s", expr);
      ret = ioctl(fd, LCD_IOCTL_WRITE, &display);
    }
  // }
  // strcpy(expr, argv[1]);
  // display.Count = sprintf((char*) display.Msg, expr);
  // ret = ioctl(fd, LCD_IOCTL_WRITE, &display);

  result = calc_recursive(expr);
  printf("%s = %d\n", expr, result);
  segment_display_decimal(fd, result);
  led_display_binary(fd, result);

  close(fd);
  return 0;
}

char interpret(unsigned short k){
  switch(k & 0xff){
    case 'A': return '+';
    case 'B': return '-';
    case 'C': return '*';
    case 'D': return '/';
    default: return (k & 0xff);
  }
}

int calc_recursive(char* operations){
  char* pos = NULL;
  char str1[100], str2[100];
  int ans1, ans2;
  if((pos = strrchr(operations, '+')) != NULL){
    strcpy(str1, operations);
    strcpy(str2, pos+1);
    str1[pos-operations] = '\0';
    printf("%s %c %s\n", str1, *pos, str2);
    ans1 = calc_recursive(str1);
    ans2 = calc_recursive(str2);
    return ans1 + ans2;
  }
  if((pos = strrchr(operations, '-')) != NULL){
    strcpy(str1, operations);
    strcpy(str2, pos+1);
    str1[pos-operations] = '\0';

    printf("%s %c %s\n", str1, *pos, str2);
    ans1 = calc_recursive(str1);
    ans2 = calc_recursive(str2);
    return ans1 - ans2;
  }
  if((pos = strrchr(operations, '*')) != NULL){
    strcpy(str1, operations);
    strcpy(str2, pos+1);
    str1[pos-operations] = '\0';

    printf("%s %c %s\n", str1, *pos, str2);
    ans1 = calc_recursive(str1);
    ans2 = calc_recursive(str2);
    return ans1 * ans2;
  }
  if((pos = strrchr(operations, '/')) != NULL){
    strcpy(str1, operations);
    strcpy(str2, pos+1);
    str1[pos-operations] = '\0';

    printf("%s %c %s\n", str1, *pos, str2);
    ans1 = calc_recursive(str1);
    ans2 = calc_recursive(str2);
    return ans1 / ans2;
  }
  return atoi(operations);
}

int segment_display_decimal(int fd, int data){
  int i;
  _7seg_info_t seg_data;
  unsigned long result_num = 0;
  for(i=1; i<=4096; i*=16){
    printf("%d^%d, ", data%10, (i));
    result_num += (data%10)*(i);
    data /= 10;
  }
  printf("%04lx\n", result_num);

  seg_data.Mode = _7SEG_MODE_HEX_VALUE;
  seg_data.Which = _7SEG_ALL;
  seg_data.Value = result_num;
  return ioctl(fd, _7SEG_IOCTL_SET, &seg_data);
}

int led_display_binary(int fd, int data){
  int i, ret=0;
  for(i=0; i<8; i++){
    if(data%2) ret |= ioctl(fd, LED_IOCTL_BIT_SET, &i);
    else ret |= ioctl(fd, LED_IOCTL_BIT_CLEAR, &i);
    data /= 2;
  }
  return ret;
}