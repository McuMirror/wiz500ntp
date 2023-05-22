#include "w5500init.h"
extern volatile uint16_t phystatus_check_cnt;
uint8_t phylink = PHY_LINK_OFF;
/////////////////////////////////////////////////////////////////
// SPI Callback function for accessing WIZCHIP                 //
// WIZCHIP user should implement with your host spi peripheral //
/////////////////////////////////////////////////////////////////
wiz_NetInfo device_net_config = { .mac = {0x00, 0x08, 0xDC,0x55, 0x00, 0x09},
                            .ip = {192, 168, 22, 165},
                            .sn = {255,255,255,0},
                            .gw = {0, 0, 0, 0},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_STATIC };



static void  wizchip_select(void)
{
	//GPIO_ResetBits(W5500_CS_GPIO_PORT, W5500_CS_PIN);
	GPIO_PinWrite(GPIOB, 12, 0);
}

static void  wizchip_deselect(void)
{
	//GPIO_SetBits(W5500_CS_GPIO_PORT, W5500_CS_PIN);
	GPIO_PinWrite(GPIOB, 12, 1);
	//stm32_spi_rw(0xFF);
}

void w5500_lib_init(void){

		uint8_t tmp;
		intr_kind temp;
		uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};	
		

		//RST low
		GPIO_PinWrite(GPIOB, 0, 0);	
		delay_ms(200);
		//////////
   // TODO //
   ////////////////////////////////////////////////////////////////////////////////////////////////////
   // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP //
   ////////////////////////////////////////////////////////////////////////////////////////////////////
   
		/* Critical section callback - No use in this example */
		//reg_wizchip_cris_cbfunc(0, 0);
			
    /* Chip selection call back */
		reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
		/* SPI Read & Write callback function */
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
		reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);
		//RST High to run
		GPIO_PinWrite(GPIOB, 0, 1);
		delay_ms(500);
    ////////////////////////////////////////////////////////////////////////
		/* WIZCHIP SOCKET Buffer initialize */
		//Initializes to WIZCHIP with SOCKET buffer size 2 or 1 dimension array typed uint8_t
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
       //printf("WIZCHIP Initialized fail.\r\n");
    }
		
		
		/* PHY link status check */
    
//		do
//    {
//       if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
//          ;//printf("Unknown PHY Link stauts.\r\n");
//    }while(tmp == PHY_LINK_OFF);
		
		//Cau hinh ngat tren S0
		temp = IK_SOCK_0;
		if(ctlwizchip(CW_SET_INTRMASK, &temp) == WZN_ERR)
		{
			//printf("Cannot set imr...\r\n");
		}
		
		Net_Conf(device_net_config);
		

}

void Display_Net_Conf()
{
	uint8_t tmpstr[6] = {0,};
	wiz_NetInfo device_net_config;

	ctlnetwork(CN_GET_NETINFO, (void*) &device_net_config);
	ctlwizchip(CW_GET_ID,(void*)tmpstr);

	// Display Network Information
	if(device_net_config.dhcp == NETINFO_DHCP) printf("\r\n===== %s NET CONF : DHCP =====\r\n",(char*)tmpstr);
		else printf("\r\n===== %s NET CONF : Static =====\r\n",(char*)tmpstr);

	printf("\r\nMAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", device_net_config.mac[0], device_net_config.mac[1], device_net_config.mac[2], device_net_config.mac[3], device_net_config.mac[4], device_net_config.mac[5]);
	printf("IP: %d.%d.%d.%d\r\n", device_net_config.ip[0], device_net_config.ip[1], device_net_config.ip[2], device_net_config.ip[3]);
	printf("GW: %d.%d.%d.%d\r\n", device_net_config.gw[0], device_net_config.gw[1], device_net_config.gw[2], device_net_config.gw[3]);
	printf("SN: %d.%d.%d.%d\r\n", device_net_config.sn[0], device_net_config.sn[1], device_net_config.sn[2], device_net_config.sn[3]);
	printf("DNS: %d.%d.%d.%d\r\n", device_net_config.dns[0], device_net_config.dns[1], device_net_config.dns[2], device_net_config.dns[3]);
	
}

void Net_Conf(wiz_NetInfo netinfo)
{
	
	ctlnetwork(CN_SET_NETINFO, (void*) &netinfo);
	Display_Net_Conf();
}
//Kiem tra W5500, SPI co on ko, day mang co cam ko?
uint8_t checkDaymang(void)
{
	uint8_t tmp;
  phylink = PHY_LINK_OFF;
	
	if(ctlwizchip(CW_GET_PHYLINK, (void*)&tmp) == -1)
	{
		#ifdef DebugEnable
		printf("Unknown PHY Link stauts.\r\n");//Loi spi???
		#endif
		return 0;
	}
	
	if(tmp == PHY_LINK_OFF) return 0;//Ko cam day mang
	
//	#ifdef DebugEnable
//	printf("Co cam day mang nhe\r\n");
//	#endif
	phylink = PHY_LINK_ON;
  return 1;  
}

