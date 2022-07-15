/**
 * @file	ioconfig.c
 * @brief	Cau hinh GPIO WIZ550web
 * @version 1.0
 * @date	03/Sep/2019
 * @par Revision
 *			03/Sep/2019 - 1.0 Release
 * @author	tuannq
 * \n\n @par 
 */
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO
#include "stm32f10x_gpio.h"
#include "ioconfig.h"
#include "stm32f10x.h"
//#include "boardutil.h"




// IWDG: STM32 Independent Watchdog Initialization
void IWDG_Config(void)
{
	
	/* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  { 
    /* IWDGRST flag set */
    /* Turn on LED0 */
    GPIO_PinWrite(GPIOC, 1, 0);
    /* Clear reset flags */
    RCC_ClearFlag();
  }
  else
  {
    /* WWDGRST flag is not set */
    /* Turn off LED0 */
    GPIO_PinWrite(GPIOC, 1, 1);
  }
	
	//RCC_LSICmd(ENABLE); //open LSI
	//while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag();
	}

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_128); // 40Khz / 128 = 0.31KHz; 1 / 0.31KHz = 3.22ms
	//IWDG_SetReload(1250); // 1s, max 0xfff
	IWDG_SetReload(0xfff); // 4095 * 3.22ms = 13185.9ms = 13 seconds; it means if IWDG was not reloaded, MCU will reset!

	//IWDG_ReloadCounter();
	IWDG_Enable();
}

/** 03/Sep/2019, tuannq
* @brief  GPIO_config cho kit WIZ550web
  * @param  GPIO_PinWrite chu y  GPIO_Pin_0 # 0???
  * @retval  
  */
void WIZ550webGPIO_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;			//Interrupt parameter interrupt priority

	//All unuse to analog
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//All unuse to analog
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//All unuse to analog
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//BUTTON => D0 => PC0 
	GPIO_PortClock   (GPIOC, true);
	GPIO_PinConfigure(GPIOC, 0, GPIO_IN_PULL_UP, GPIO_MODE_INPUT);
	GPIO_PinWrite(GPIOC, 0, 1);
	
	//EX LED => D1 => PC1 
	GPIO_PortClock   (GPIOC, true);
	GPIO_PinConfigure(GPIOC, 1, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
	GPIO_PinWrite(GPIOC, 1, 1);
	
	// PB12 W5500_CS 
	GPIO_PortClock   (GPIOB, true);
	GPIO_PinConfigure(GPIOB, 12, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinWrite(GPIOB, 12, 1);
	
	// PB0 W5500_RST 
	GPIO_PinConfigure(GPIOB, 0, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
	GPIO_PinWrite(GPIOB, 0, 1);
	
	// PB1 W5500_INT 
	GPIO_PinConfigure(GPIOB, 1, GPIO_IN_PULL_UP, GPIO_MODE_INPUT);
	// Enable the EXTI0 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;				//W5500_INT External interrupt channel
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//Subpriority 1s
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//Enable external interrupt channel
	NVIC_Init(&NVIC_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// Connect EXTI Line0 to PB1 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
  
	
	// PA3 as W5500 interrupt input 
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	
	/* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  { 
    /* WWDGRST flag set */
    /* Turn on LED0 */
    GPIO_PinWrite(GPIOC, 1, 0);
    /* Clear reset flags */
    RCC_ClearFlag();
  }
  else
  {
    /* WWDGRST flag is not set */
    /* Turn off LED0 */
    GPIO_PinWrite(GPIOC, 1, 1);
  }
	
	
}
