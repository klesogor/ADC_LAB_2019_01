#include "buffer.h"
#include "delay.h"
#include "ADC.h"
#include "tm1637.h"
#include "stdio.h"
#define BUFFER_SIZE 64

unsigned int digit_display0 = 0;
struct Buffer buf;

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
       for(int i = 0;i < BUFFER_SIZE/4; i++){
				cbuf_write(&buf, ADC_read());
			}
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    }
}

int main()
{
	ADC_init();
	delay_ms(10);
	TM1637_init();
	TM1637_brightness(BRIGHT_TYPICAL);
	delay_ms(10);
	TM1637_display_all(digit_display0);
	delay_ms(1000);
	buf = cbuf_new(BUFFER_SIZE);
	
	TIM_TimeBaseInitTypeDef TIMER_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
 
  TIM_TimeBaseStructInit(&TIMER_InitStructure);
  TIMER_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIMER_InitStructure.TIM_Prescaler = 7200;
  TIMER_InitStructure.TIM_Period = 5000;
  TIM_TimeBaseInit(TIM4, &TIMER_InitStructure);
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
 
    /* NVIC Configuration */
    /* Enable the TIM4_IRQn Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	while (1)
	{
		TM1637_display_all(cbuf_avg(&buf));
		delay_ms(25);
	}
}
