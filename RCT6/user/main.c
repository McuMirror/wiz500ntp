/*
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
**/
//#define HSE_VALUE    ((uint32_t)12000000) /*!< Value of the External oscillator in Hz */
//
#include "main.h"
time_t timenow = 1566444407;
volatile uint8_t sec_cnt = 0;
int8_t ledstt;
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
	w5500_lib_init();


	
	printf("Run, now is %s\r\n",ctime(&timenow));
	//Get time from ntp time server
	SNTP_init();
	
	
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

		/**********************************************************************/
		
		if(sec_cnt > 1)
		{
			sec_cnt = 0;
			//timeinfo = localtime( &timenow );
			//printf("sec_cnt :%d, timenow :%d\r\n",sec_cnt,timenow);
			SNTP_run();
			//GPIO_PinWrite(GPIOA, 8, 1);
			
			
		}
		/**********************************************************************/
		//LED blinky : CPU run  
		
		if((msec_cnt < 100) &&(ledstt != 1)) {ledstt = 1;GPIO_PinWrite(GPIOC, 1, 0);}
		else if((msec_cnt >= 100) &&(ledstt != 0)) {ledstt = 0 ; GPIO_PinWrite(GPIOC, 1, 1);}
		/**********************************************************************/
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
		printf("Right eeprom data, load configs now\r\n");
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
		printf("Wrong eeprom data\r\n");
		EE_WriteVariable(0,123);
		EE_WriteVariable(1,456);
		EE_WriteVariable(2,789);
		//IP 192.168.1.246
		EE_WriteVariable(4,192);
		EE_WriteVariable(5,168);
		EE_WriteVariable(6,1);
		EE_WriteVariable(7,246);
		//GW: 192.168.1.1
		EE_WriteVariable(8,192);
		EE_WriteVariable(9,168);
		EE_WriteVariable(10,1);
		EE_WriteVariable(11,1);
		//SN 255.255.255.0
		EE_WriteVariable(12,255);
		EE_WriteVariable(13,255);
		EE_WriteVariable(14,255);
		EE_WriteVariable(15,0);
	}
		

}


void networkSevices(void)
{
	int32_t ret = 0;	
	checklink();//Kiem tra day mang con cam ko
	// NTP UDP server chay dau tien cho nhanh
	if( (ret = NTPUDP(SOCK_UDPS)) < 0) {
			printf("SOCKET ERROR : %d\r\n", ret);
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
    	// [Command] Get:			  snmpget -v 1 -c public 192.168.1.246 .1.3.6.1.2.1.1.1.0 			// (sysDescr)
    	// [Command] Get: 			snmpget -v 1 -c public 192.168.1.246 .1.3.6.1.4.1.6.1.0 			// (Custom, get LED status)
    	// [Command] Get-Next: 	snmpwalk -v 1 -c public 192.168.1.246 .1.3.6.1
			// [Command] Set: 			snmpset -v 1 -c public 192.168.1.246 .1.3.6.1.4.1.6.1.1 i 1			// (Custom, LED 'On')
    	// [Command] Set: 			snmpset -v 1 -c public 192.168.1.246 .1.3.6.1.4.1.6.1.1 i 0			// (Custom, LED 'Off')
			snmpd_run();	
	}
	{	// web server 	
			httpServer_run(0);
			httpServer_run(1);
			httpServer_run(2);
		}
}
/**********************************************************************/
// Chinh gio he thong theo ban tin GPS
void configTimeFollowGPS(void)
{//=> Ban tin GPS: $GPS034007060819AA10 
	
}
//Xu ly ban tin GPS
void usart1Process(void)
{
	//UART1 RX process
			if(u1out == ONTIME)
			{
				u1out = STOP;// Da nhan du ban tin UART => Xy ly
				printf("UART1:%s\r\n",USART1_rx_data_buff);
				configTimeFollowGPS();
				for(USART1_index=0;USART1_index<RX_BUFFER_SIZE0;USART1_index++)
															{
															USART1_rx_data_buff[USART1_index]=0;
															}  
															USART1_index=0;
			}
}




