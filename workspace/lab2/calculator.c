#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define ms 10000
char stack1[ms];  //堆疊陣列 
int stack2[ms];
int top=-1;

int semp()       //副函式 - 判對堆疊是不是空的 
{
  if(top == -1)
  return 1;
  else
  return 0;    
}

int push1(int d)  //副函式 - 把資料存入堆疊 
{
  if(top >= ms)
  {
    printf("Stack is full.\n");
    return 0;
  }
  else
  {
    stack1[++top] = d;//存入堆疊 
    return 1;
  }    
}

int pop1()         //副函式 - 從堆疊中取出資料 
{
  if(semp())
  return -1;
  else
  return stack1[top--];//取出資料    
}

int push2(int d)   //副函式 - 把資料存入堆疊 
{
  if(top >= ms)
  {
    printf("Stack is full.\n");
    return 0;
  }
  else
  {
    stack2[++top] = d;//存入堆疊 
    return 1;
  }    
}
int pop2()         //副函式 - 從堆疊中取出資料 
{
  if(semp())
  return -1;
  else
  return stack2[top--];//取出資料    
}


int oper(char op)    //副函式 - 判斷符號是否為運算子 
{
  switch (op)
  {
    case '*':
    case '/':
    case '%':
    case '+':
    case '-':
    case '^':  
      return 1;//是運算子
    default: 
      return 0;//非運算子 
  }    
}

int priority(char op)  //副函式 - 判斷優先權, 運算子優先權越高回傳值越大 
{
  switch (op)
  {
    case '^':
      return 4;
    case '*':
    case '/':
    case '%':
      return 3;
    case '+':
    case '-':
      return 2;
    case '(':
      return 1;
    default: 
      return 0;    
    }    
}

int cal(int op, long int n1, long int n2)   //副函式 - 計算算式
{
  int f;
  switch ( (char) op)
  {
    case '*': return(n1*n2);
    case '/': return(n2/n1); 
    case '%': return(n2%n1);
    case '+': return(n1+n2);
    case '-': return(n2-n1);          
  }    
}  

int calpostfix(char *num)    //副函式 - 將後序算式的值計算出來
{
  long int ch1=0, ch2=0;
  int time=0;
  while(num[time] != '\0' && num[time] != '\n')
  {
    if(oper(num[time]))
    {
      ch1 = pop2();
      ch2 = pop2();
      push2(cal(num[time], ch1, ch2));
    }
    else
    push2(num[time]-48);      //字元轉成數字
    time++;
  }    
  return pop2();
} 
 
int main()
{
  char t, inf[ms], post[ms];
  int time1=0, time2=0;
  printf("這是一台計算機可以使用下列用法:\n");
  printf("+: 加法.   -: 減法.\n*: 乘法.   /: 除法.\n"); 
  printf("%%: 取餘數. ^: 取次方.\n(: 左括號. ): 右括號.\n\n");
  printf("以下是各符號的優先權:\n ^ > * = / = %% = > + = - > (.\n\n"); 
  printf("請輸入所要運算的算式:\n");
  scanf("%s", inf);
  while((t = inf[time1++])!= '\0')
  {
    switch(t)
    {
      case '(': push1(t);      //碰到左括號時，將值存入堆疊中 
        break;
      case ')':
        while(stack1[top] != '(')
        post[time2++] = pop1(); //處理括號內的運算子，將運算子全部取出來直到遇到左括號為止，      
        pop1();                 //之後把運算子存到後序式中，然後再把左括號取出來捨棄 
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
      case '^':
        while(!semp() && priority(t) <= priority(stack1[top]))
        post[time2++] = pop1();     
        push1(t);
        break;                  //當不為空堆疊時比較優先權，然後再決定將運算子存入後序式或存入堆疊 
      default: 
        post[time2++] = t;     //當中序式元素是運算元時直接存入後序式 
    }
  }
  while(!semp())
  post[time2++] = pop1();      //若堆疊不是空的，則取出剩下的運算子 
  printf("\n此算式的解答為:\n%d\n", calpostfix(post));
  return 0;
}
