/*
NOTE: Da chuyen U1 ve 9600
- 9/Jan/2020: fix loi thoi gian nhan vao qua uart la UTC nhung thoi gian gui qua NTP la UTC + 7
							Them web config cua Tung va fullconfig.html
- 3/Jan/2020: Thoi gian GPS la UTC nen ko can tru 7h 
- Neu thoi gian cua module ma khac gio that qua nhieu thi win10 se ko cap nhat vi thoi gian khac nhieu qua
- 25/Dec/2019:
 + Add factory reset system
 + If lost master message, update to webserver => lost signal
- 24/Dec/2019: 
 + Sua loi phan nhan ban tin UART, cap nhat sai gio.
 + Time ref chuyen vao phan nhan ban tin UART
 + Add timeout of GPS master message
 + Cai tien phan kiem tra day mang co cam hay ko?
 + Them phan timeout of master vao SNMP table
- 20/Dec/2019: Fix mot so loi
	+ Phai chuyen toc do UART thanh 9600 de phu hop voi dong ho GPS clock
- 23/Oct/2019 : Make new project on NTP PCB new, STM32F103RBT6. Check...
								redefine STM32F10X_MD, STM32F10X_HD with STM32F103RCT6 and flash config!!!!!!!!!!!!!
								HSE = 8MHz
- 03/Sep/2019 : Make new project on STM32F103RCT6 WIZ550web https://www.wiznet.io/product-item/wiz550web/
- Xtal : 12Mhz trong file system_stm32f10x.c thay doi SetSysClockTo72() : RCC_CFGR_PLLMULL9 => RCC_CFGR_PLLMULL6 do XT = 12Mhz
- kiem tra printmcuclk() dam bao :
ADCCLK:36000000
HCLK:72000000
PCLK1:36000000
PCLK2:72000000
SYSCLK:72000000

- UART1 : debug ok
- STM32 Independent WatchDog : 13s
- Timer_Configuration() :
- LED 0 : CPU run, blinky one per second
- NTP time server
- SMNP Agent
- Get time from ntp server
- httpServer


code nap 8/Oct/2021
**/
//#define HSE_VALUE    ((uint32_t)12000000) /*!< Value of the External oscillator in Hz */
//
#include "main.h"
//#define _U1_DEBUG_ENABLE_

time_t timenow = 1633682285;
volatile uint8_t sec_cnt = 0;
int8_t ledstt;
// lost signal : if after timeOutLostSignal seconds without GPS master message => lost
int8_t lostSignal = LOST_GPS_MASTER;
int8_t timeOutLostSignal = 30;
uint8_t gps1_stt = 0;
uint8_t gps2_stt = 0;
uint8_t power1_stt = 0;
uint8_t power2_stt = 0;
uint8_t days = 0;
uint8_t months = 0;
uint8_t years = 0;
uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;
struct tm currtime;
void factoryRST(void);
/**************************************************************************/
int main(void)
{	
	int32_t ret = 0;

	
  SystemInit();
	SystemCoreClockUpdate();
	/* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))
  { 
    /* Capture error */ 
    while (1);
  }


	
	//Thu vien tang thap
	WIZ550webGPIO_config();
	Timer_Configuration();
	SPI2_Init();
	USART1_Init();
	//printmcuclk();
	
	//using stm32's flash to store data
	//EEPROM STM32 init
	sw_eeprom_stm32();
	test_eeprom();
	//delay_ms(1);
	//Thu vien tang cao
	//Neu ham nay ko hoat dong: kiem tra cac chan GPIO khai bao!
	w5500_lib_init();


	#ifdef _U1_DEBUG_ENABLE_
	printf("Run, now is %s\r\n",ctime(&timenow));
	#endif
	#ifdef _GET_TIME_INTERNET_ENABLE_
	//Get time from ntp time server
	SNTP_init();
	#endif
	
	ntpserverdefaultconfig();
	/* SNMP(Simple Network Management Protocol) Agent Initialize */
	// NMS (SNMP manager) IP address
	snmpd_init(managerIP,agentIP,SOCK_agent,SOCK_trap);	
	loadwebpages();
	
	// IWDG Initialization: STM32 Independent WatchDog
	IWDG_Config();
	//timeinfo = localtime( &timenow );
	//printf("Current local time and date: %s\r\n", asctime(timeinfo));
	/*****************************MAIN WHILE************************************/
	while(1)
	{
		IWDG_ReloadCounter(); // Feed IWDG
		/**********************************************************************/
		networkSevices();
		/**********************************************************************/
		if(GPIO_PinRead(GPIOC,0) ==0 ) 
		{
			delay_ms(2000);
			if(GPIO_PinRead(GPIOC,0) ==0 ) printf("Reset factory setting\r\n");
			factoryRST();
			NVIC_SystemReset();
		}
		/**********************************************************************/
		// Tu chay dong ho va cap nhat thoi gian tu Internet
		if(sec_cnt >= 1)
		{
			sec_cnt = 0;
			#ifdef _GET_TIME_INTERNET_ENABLE_
			//timeinfo = localtime( &timenow );
			//printf("sec_cnt :%d, timenow :%d\r\n",sec_cnt,timenow);
			//Neu ko co mang cai nay se gay treo??? 19/Sep/2019
			SNTP_run();
			//GPIO_PinWrite(GPIOA, 8, 1);
			seconds++;
			if(seconds == 60) seconds = 0;
			#endif

			if(timeOutLostSignal) timeOutLostSignal--;
			if(timeOutLostSignal == 0) lostSignal = LOST_GPS_MASTER;
		}
		/**********************************************************************/
		//LED blinky : CPU run, neu ko nhay thi la co van de!  
		
		if((msec_cnt < 100) &&(ledstt != 1)) {ledstt = 1;GPIO_PinWrite(GPIOC, 1, 0);}
		else if((msec_cnt >= 100) &&(ledstt != 0)) {ledstt = 0 ; GPIO_PinWrite(GPIOC, 1, 1);}
		/**********************************************************************/
		//Xu ly thoi gian tu mach GPS master gui sang, neu ko nhan dc thi phai bao TIMEOUT
		usart1Process();
		
		

	}//end of main while

		
}//end of main

/**
  * @brief  test_eeprom
  * @param  Call this fuction for test store data to eeprom
  * @retval  
  */	
void test_eeprom(void)
{
	uint16_t a,b,c,d; 
	uint16_t confirm[3];
	
	//Kiem tra confirm xem dung ko, neu sai thi du lieu bi sai => reset factory setting
	// Neu dung thi load config
	
	EE_ReadVariable(0,&confirm[0]);
	EE_ReadVariable(1,&confirm[1]);
	EE_ReadVariable(2,&confirm[2]);	
	
	if( (confirm[0] == 123) && (confirm[1] == 456) && (confirm[2] == 789))
	{
		#ifdef _U1_DEBUG_ENABLE_
		printf("Right eeprom data, load configs now\r\n");
		#endif
		//Load IP
		EE_ReadVariable(4,&a);
		EE_ReadVariable(5,&b);
		EE_ReadVariable(6,&c);
		EE_ReadVariable(7,&d);
		gWIZNETINFO.ip[0] = a;
		gWIZNETINFO.ip[1] = b;
		gWIZNETINFO.ip[2] = c;
		gWIZNETINFO.ip[3] = d;
		//printf("Load ip: %d.%d.%d.%d",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
		//Load GW
		EE_ReadVariable(8,&a);
		EE_ReadVariable(9,&b);
		EE_ReadVariable(10,&c);
		EE_ReadVariable(11,&d);
		gWIZNETINFO.gw[0] = a;
		gWIZNETINFO.gw[1] = b;
		gWIZNETINFO.gw[2] = c;
		gWIZNETINFO.gw[3] = d;
		//Load SN
		EE_ReadVariable(12,&a);
		EE_ReadVariable(13,&b);
		EE_ReadVariable(14,&c);
		EE_ReadVariable(15,&d);
		gWIZNETINFO.sn[0] = a;
		gWIZNETINFO.sn[1] = b;
		gWIZNETINFO.sn[2] = c;
		gWIZNETINFO.sn[3] = d;
	}
	else
	{
		#ifdef _U1_DEBUG_ENABLE_
		printf("Wrong eeprom data\r\n");
		#endif
		EE_WriteVariable(0,123);
		EE_WriteVariable(1,456);
		EE_WriteVariable(2,789);
		//IP 192.168.22.165
		EE_WriteVariable(4,192);
		EE_WriteVariable(5,168);
		EE_WriteVariable(6,22);
		EE_WriteVariable(7,165);
		gWIZNETINFO.ip[0] = 192;
		gWIZNETINFO.ip[1] = 168;
		gWIZNETINFO.ip[2] = 22;
		gWIZNETINFO.ip[3] = 165;
		//GW: 192.168.22.1
		EE_WriteVariable(8,192);
		EE_WriteVariable(9,168);
		EE_WriteVariable(10,22);
		EE_WriteVariable(11,252);
		gWIZNETINFO.gw[0] = 192;
		gWIZNETINFO.gw[1] = 168;
		gWIZNETINFO.gw[2] = 22;
		gWIZNETINFO.gw[3] = 252;
		//SN 255.255.255.0
		EE_WriteVariable(12,255);
		EE_WriteVariable(13,255);
		EE_WriteVariable(14,255);
		EE_WriteVariable(15,0);
		gWIZNETINFO.sn[0] = 255;
		gWIZNETINFO.sn[1] = 255;
		gWIZNETINFO.sn[2] = 255;
		gWIZNETINFO.sn[3] = 0;
	}
		

}

void factoryRST(void)
	{
		#ifdef _U1_DEBUG_ENABLE_
		printf("Factory reset!\r\n");
		#endif
		EE_WriteVariable(0,123);
		EE_WriteVariable(1,456);
		EE_WriteVariable(2,789);
		//IP 192.168.1.165
		EE_WriteVariable(4,192);
		EE_WriteVariable(5,168);
		EE_WriteVariable(6,22);
		EE_WriteVariable(7,165);
		
		//GW: 192.168.1.1
		EE_WriteVariable(8,192);
		EE_WriteVariable(9,168);
		EE_WriteVariable(10,22);
		EE_WriteVariable(11,252);
		
		//SN 255.255.255.0
		EE_WriteVariable(12,255);
		EE_WriteVariable(13,255);
		EE_WriteVariable(14,255);
		EE_WriteVariable(15,0);
		
	}
	
void networkSevices(void)
{
	int32_t ret = 0;	
	checklink();//Kiem tra day mang con cam ko
	// NTP UDP server chay dau tien cho nhanh
	if( (ret = NTPUDP(SOCK_UDPS)) < 0) {
			printf("SOCKET ERROR : %d\r\n", ret);
			NVIC_SystemReset();
	}
	{	//SNMPv1 run
			//Run SNMP Agent Fucntion
			/* SNMP Agent Handler */
			//SMI Network Management Private Enterprise Codes: : moi cong ty phai dang ky 1 so rieng, 
			//tham khao : https://www.iana.org/assignments/enterprise-numbers/enterprise-numbers
			// Vi du Arduino : 36582
    	// SNMP Agent daemon process : User can add the OID and OID mapped functions to snmpData[] array in snmprun.c/.h
			// [net-snmp version 5.7 package for windows] is used for this demo.
			// * Command example
    	// [Command] Get:			  snmpget -v 1 -c public 192.168.1.165 .1.3.6.1.2.1.1.1.0 			// (sysDescr)
    	// [Command] Get: 			snmpget -v 1 -c public 192.168.1.165 .1.3.6.1.4.1.6.1.0 			// (Custom, get LED status)
    	// [Command] Get-Next: 	snmpwalk -v 1 -c public 192.168.1.165 .1.3.6.1
			// [Command] Set: 			snmpset -v 1 -c public 192.168.1.165 .1.3.6.1.4.1.6.1.1 i 1			// (Custom, LED 'On')
    	// [Command] Set: 			snmpset -v 1 -c public 192.168.1.165 .1.3.6.1.4.1.6.1.1 i 0			// (Custom, LED 'Off')
			snmpd_run();	
	}
	{	// web server 	
			httpServer_run(0);
			httpServer_run(1);
			httpServer_run(2);
		}
}
/**********************************************************************/
//Ham chuyen doi char sang int
uint8_t convert_atoi( uint8_t c)
{
	return (uint8_t)c-48;
}
//Xu ly ban tin GPS
void GPS_message_handle()
{//=> Ban tin GPS: $GPS034007060819AA10	
	if((USART1_rx_data_buff[0] =='$')&((USART1_rx_data_buff[1] =='G')|(USART1_rx_data_buff[1] =='g'))&((USART1_rx_data_buff[2] =='P')|(USART1_rx_data_buff[2] =='p'))&((USART1_rx_data_buff[3] =='S')|(USART1_rx_data_buff[3] =='s')))
	{
		/*Truyen gia tri gui len web server*/
		//If there is not GPS master message, no time on webserver
		days 		= 10*convert_atoi(USART1_rx_data_buff[10])+convert_atoi(USART1_rx_data_buff[11]);
		months 	= 10*convert_atoi(USART1_rx_data_buff[12])+convert_atoi(USART1_rx_data_buff[13]);
		years 	= 10*convert_atoi(USART1_rx_data_buff[14])+convert_atoi(USART1_rx_data_buff[15]);
		hours 	= 10*convert_atoi(USART1_rx_data_buff[4])+convert_atoi(USART1_rx_data_buff[5])  ;//UTC
		minutes = 10*convert_atoi(USART1_rx_data_buff[6])+convert_atoi(USART1_rx_data_buff[7]);
		seconds = 10*convert_atoi(USART1_rx_data_buff[8])+convert_atoi(USART1_rx_data_buff[9]);
			
		/*Cap nhap thoi gian NTP*/
		currtime.tm_year = 100+ years;//100+10*convert_atoi(USART1_rx_data_buff[14])+convert_atoi(USART1_rx_data_buff[15]);//In fact: 2000+xxx-1900
		currtime.tm_mon  = months-1;//10*convert_atoi(USART1_rx_data_buff[12])+convert_atoi(USART1_rx_data_buff[13])-1;
		currtime.tm_mday = days;//10*convert_atoi(USART1_rx_data_buff[10])+convert_atoi(USART1_rx_data_buff[11]);
		
		currtime.tm_sec  = seconds;//10*convert_atoi(USART1_rx_data_buff[8])+convert_atoi(USART1_rx_data_buff[9]);
		currtime.tm_min  = minutes;//10*convert_atoi(USART1_rx_data_buff[6])+convert_atoi(USART1_rx_data_buff[7]);
		currtime.tm_hour = hours;//10*convert_atoi(USART1_rx_data_buff[4])+convert_atoi(USART1_rx_data_buff[5]);
		timenow = mktime(&currtime);
		timenow = timenow - 25200;//Tru di 7 tieng
		timeOutLostSignal = 30;//seconds 
		lostSignal = GPS_MASTER_OK;
		
		#ifdef _U1_DEBUG_ENABLE_
		printf("new timestamp:%d\r\n",timenow);
		#endif
		//Update last sync NTP time server field!
		unixTime_last_sync = timenow + STARTOFTIME;
		unixTime_last_sync = htonl(unixTime_last_sync);
		memcpy(&serverPacket[16], &unixTime_last_sync, 4);
		
		//Update SNMP data table
		if(USART1_rx_data_buff[16]=='A') gps1_stt = 1;
		else gps1_stt = 0;
		if(USART1_rx_data_buff[17]=='A') gps2_stt = 1;
		else gps2_stt = 0;
		if(USART1_rx_data_buff[18]=='1') power1_stt = 1;
		else power1_stt = 0;
		if(USART1_rx_data_buff[19]=='1') power2_stt = 1;
		else power2_stt = 0;
	}
}
//Xu ly ban tin GPS
void usart1Process(void)
{
	//UART1 RX process
			if(u1out == ONTIME)
			{
				u1out = STOP;// Da nhan du ban tin UART => Xy ly
				#ifdef _U1_DEBUG_ENABLE_
				printf("UART1:%s\r\n",USART1_rx_data_buff);
				#endif
				GPS_message_handle();
				for(USART1_index=0;USART1_index<RX_BUFFER_SIZE0;USART1_index++)
															{
															USART1_rx_data_buff[USART1_index]=0;
															}  
															USART1_index=0;
			}
}




