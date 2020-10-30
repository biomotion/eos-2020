#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int calc(char op, int n1, int n2)
{
  switch (op)
  {
    case '*': return(n1*n2);
    case '/': return(n2/n1); 
    case '+': return(n1+n2);
    case '-': return(n2-n1);          
  } 
  return -1;  
}  

int calc_recursive(char* operations){
  char* pos = NULL;
  int index = 0;
  char str1[20], str2[20];
  if(
    ((pos = strchr(operations, '+')) != NULL) ||
    ((pos = strchr(operations, '-')) != NULL) ||
    ((pos = strchr(operations, '*')) != NULL) ||
    ((pos = strchr(operations, '/')) != NULL)
  ){
    index = (int)(pos - operations);
    // printf("%p %d\n", pos, index);
    strcpy(str1, operations);
    strcpy(str2, pos+1);
    str1[index] = '\0';
    printf("%s %c %s\n", str1, operations[index], str2);
    return calc(operations[index], calc_recursive(str1), calc_recursive(str2));
  }
  return atoi(operations);
}

int main(int argc, char** argv){
  int result;
  if(argc > 1){
    printf("%s\n", argv[1]);
    result = calc_recursive(argv[1]);
  }else
    return 0;
  printf("The answer is: %d\n", result);
  return 0;
}