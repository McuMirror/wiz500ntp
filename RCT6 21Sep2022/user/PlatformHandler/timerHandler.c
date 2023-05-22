/**
 * @file	timerHandler.c
 * @brief	config timer
 - TIM2 : 1kHz 
 - TIM3 : 10kHz
 * @version 1.0
 * @date	03/Sep/2019
 * @par Revision
 *			03/Sep/2019 - 1.0 Release
 * @author	tuannq
 * \n\n @par 
 */

#include "timerHandler.h"
#include "time.h"
struct tm* timeinfo;

extern uint16_t t_check_link_ms;
extern volatile uint32_t snmp_tick_1ms;
volatile uint16_t msec_cnt = 0;
extern volatile uint8_t sec_cnt;
extern time_t timenow;
/*---------------------------------------------*/
/* 1kHz timer process                          */
/*---------------------------------------------*/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET )
	{
		// Also cleared the wrong interrupt flag in the ISR
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // Clear the interrupt flag

		//Todo
		//Timer++;	/* Increment performance counter */
		if (u1out > ONTIME) u1out--;
		snmp_tick_1ms++;
		t_check_link_ms++;
		
		msec_cnt++; // milli second
		if(msec_cnt >= 999) // second
		{
			sec_cnt++;
			msec_cnt = 0;
		}
	}
}
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET )
	{
		// Also cleared the wrong interrupt flag in the ISR
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // Clear the interrupt flag
		timenow++;
	}
}
/**
  * @brief  Configures the Timer
  * @param  None
  * @return None
  */
void Timer_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//tim uptade frequency = TIM_CLK/(TIM_PSC+1)/(TIM_ARR + 1)
	//TIM_CLK/(TIM_Period + 1) /(Prescaler+1)
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 999;// count 1000 tick then interrupt, gia tri max cua bo dem
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM2, 72-1, TIM_PSCReloadMode_Immediate);

  /* TIM enable counter */
  TIM_Cmd(TIM2, ENABLE);

  /* TIM IT enable */
  
	/***************************************************************************************************************/
	//	16/Aug/2019, Tuan, use Tim3 for make a couter 0-0.9999 second for NTP fraction
	//TIM3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	//tim uptade frequency = TIM_CLK/(TIM_PSC+1)/(TIM_ARR + 1)
	//TIM_CLK/(TIM_Period + 1) /(Prescaler+1)
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 9999;// count 9999 tick then interrupt, gia tri max cua bo dem
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM3, 7200-1, TIM_PSCReloadMode_Immediate);

  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);
	/* TIM IT enable */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

}

