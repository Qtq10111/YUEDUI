#include "delay.h"
#include "main.h"

void delay_us(uint16_t delay_us)//微秒计时
{    
  volatile unsigned int num;
  volatile unsigned int t;
 
  
  for (num = 0; num < delay_us; num++)
  {
    t = 11;
    while (t != 0)
    {
      t--;
    }
  }
}

