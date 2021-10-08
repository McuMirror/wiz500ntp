/**
 * @file	timerHandler.h
 * @brief	config timer
 * @version 1.0
 * @date	03/Sep/2019
 * @par Revision
 *			03/Sep/2019 - 1.0 Release
 * @author	tuannq
 * \n\n @par 
 */

#ifndef __TIMERHANDLER_H
#define __TIMERHANDLER_H
#include "stm32f10x.h"
#include "time.h"
#include "boardutil.h"
#define ONTIME 1
#define STOP   0
//UART1 receiver timeout
extern __IO uint32_t u1out;// 50 is 50ms
extern volatile uint16_t msec_cnt;
extern volatile uint8_t sec_cnt;
extern volatile uint32_t snmp_tick_1ms;
extern struct tm* timeinfo;
void Timer_Configuration(void);

#endif

