/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"                  // Device header
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO
#include <time.h>
#include "ff.h"
#include "diskio.h"
//user include
#include "ioconfig.h"
#include "uartHandler.h"
#include "stm32f10x_it.h"
#include "timerHandler.h"
#include "delay.h"
#include "spiconfig.h"
#include "w5500init.h"
#include "stm32f1fatfs.h"
#include "socketdefines.h"
#include "ntpserver.h"
#include "snmp.h"
#include "eeprom_stm.h"
#include "httpServer.h"
#include "webpage.h"
#ifdef _GET_TIME_INTERNET_ENABLE_
#include "sntp.h"
#endif
#include "dns.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//#define ONTIME 1
//#define STOP   0
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void networkSevices(void);
void loadwebpages(void);
void usart1Process(void);
void test_eeprom(void);
#define GPS_MASTER_OK   1
#define LOST_GPS_MASTER 0
#endif /* __MAIN_H */

