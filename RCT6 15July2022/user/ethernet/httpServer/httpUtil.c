/**
 * @file	httpUtil.c
 * @brief	HTTP Server Utilities	
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */
#include "stm32f10x.h"                  // Device header
#include "wizchip_conf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpUtil.h"
#include "eeprom_stm.h"//save IP
#include "main.h"
extern wiz_NetInfo device_net_config;
/************************************************************************************************/


//Bien luu gia tri cho webserver
extern uint8_t gps1_stt;
extern uint8_t gps2_stt;
extern uint8_t power1_stt;
extern uint8_t power2_stt;
extern uint8_t days;
extern uint8_t months;
extern uint8_t years;
extern uint8_t hours;
extern uint8_t minutes;
extern uint8_t seconds;
extern int8_t lostSignal;
// Pre-defined Get CGI functions
void make_json_netinfo(uint8_t * buf, uint16_t * len);


uint8_t new_device_ip[4];
void make_json_netinfo2(uint8_t * buf, uint16_t * len)
{
	wiz_NetInfo netinfo;
	ctlnetwork(CN_GET_NETINFO, (void*) &netinfo);

	// DHCP: 1 - Static, 2 - DHCP
	*len = sprintf((char *)buf, "NetinfoCallback({\"mac\":\"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\",\
											\"ip\":\"%d.%d.%d.%d\",\
											\"gw\":\"%d.%d.%d.%d\",\
											\"sn\":\"%d.%d.%d.%d\",\
											\"dns\":\"%d.%d.%d.%d\",\
											\"dhcp\":\"%d\"\
											});",
											netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5],
											netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3],
											netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3],
											netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3],
											netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3],
											netinfo.dhcp
											);
}
/************************************************************************************************/

/************************************************************************************************/
uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len)
{
	uint8_t ret = 1;	// ret = '1' means 'uri_name' matched
	uint8_t val = 0;

	if(strcmp((const char *)uri_name, "set_diodir.cgi") == 0)
	{
		//val = set_diodir(uri);
		*len = sprintf((char *)buf, "%d", val);
	}
	else
	{
		ret = 0;
		//printf("predefined_set_cgi_processor not found\r\n");
	}

	return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t http_get_json_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;

//	if(predefined_get_cgi_processor(uri_name, buf, &len))
//	{
//		;
//	}
//	else 
	if(strcmp((const char *)uri_name, "alldata.json") == 0)
	{
		// To do
		//printf("uri_name : %s\r\n",uri_name);
		make_json_netinfo(buf, &len);
		;
	}
//	else if(strcmp((const char *)uri_name, "get_serial_data.cgi") == 0)
//	{
//		//make_json_serial_data(buf, &len);
//	}
	else
	{
		// CGI file not found
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}
uint8_t http_get_cgi_handler(uint8_t * uri_name, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;

//	if(predefined_get_cgi_processor(uri_name, buf, &len))
//	{
//		;
//	}
//	else 
//
	if(strcmp((const char *)uri_name, "get_netinfo.cgi") == 0)
	{
		make_json_netinfo2(buf, &len);
		//printf("get_netinfo.cgi\r\n");
	}	
	else if(strcmp((const char *)uri_name, "example.cgi") == 0)
	{
		// To do
		;
	}
//	else if(strcmp((const char *)uri_name, "get_serial_data.cgi") == 0)
//	{
//		//make_json_serial_data(buf, &len);
//	}
	else
	{
		// CGI file not found
		//printf("CGI file not found\r\n");
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t * set_basic_config_setting(uint8_t * uri)
{
	uint8_t * param;
	uint8_t * ip = device_net_config.ip;
  
	//boc tach IP
		if((param = get_http_param_value((char *)uri, "ip")))
		{
			inet_addr_((uint8_t*)param, device_net_config.ip);
			//printf("IP: %d.%d.%d.%d\r\n",return_ip[0],return_ip[1],return_ip[2],return_ip[3]);
		}
		//boc tach Getway
		if((param = get_http_param_value((char *)uri, "gw")))
		{
			inet_addr_((uint8_t*)param, device_net_config.gw);
			
		}
		//boc tach Subnet
		if((param = get_http_param_value((char *)uri, "sn")))
		{
			inet_addr_((uint8_t*)param, device_net_config.sn);
			
		}
		

	return ip;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void make_cgi_basic_config_response_page(uint16_t delay, uint8_t * url, uint8_t * cgi_response_buf, uint16_t * len)
{
	*len = sprintf((char *)cgi_response_buf,"<html><head><title>Network Configuration</title><script language=javascript>j=%d;function func(){document.getElementById('delay').innerText=' '+j+' ';if(j>0)j--;setTimeout('func()',1000);if(j<=0)location.href='http://%d.%d.%d.%d';}</script></head><body onload='func()'>Please wait for a while, the module will boot in<span style='color:red;' id='delay'></span> seconds.</body></html>", delay, url[0], url[1], url[2], url[3]);
	//printf("\r\n%s\r\n",cgi_response_buf);
	return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void saveip(){
		
		//printf("Save new eeprom data\r\n");
		EE_WriteVariable(0,123);
		EE_WriteVariable(1,456);
		EE_WriteVariable(2,789);
		//IP 192.168.1.246
		EE_WriteVariable(4,device_net_config.ip[0]);
		EE_WriteVariable(5,device_net_config.ip[1]);
		EE_WriteVariable(6,device_net_config.ip[2]);
		EE_WriteVariable(7,device_net_config.ip[3]);
		//GW: 192.168.1.1
		EE_WriteVariable(8, device_net_config.gw[0]);
		EE_WriteVariable(9, device_net_config.gw[1]);
		EE_WriteVariable(10,device_net_config.gw[2]);
		EE_WriteVariable(11,device_net_config.gw[3]);
		//SN 255.255.255.0
		EE_WriteVariable(12,device_net_config.sn[0]);
		EE_WriteVariable(13,device_net_config.sn[1]);
		EE_WriteVariable(14,device_net_config.sn[2]);
		EE_WriteVariable(15,device_net_config.sn[3]);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t http_post_cgi_handler(uint8_t * uri_name, st_http_request * p_http_request, uint8_t * buf, uint32_t * file_len)
{
	uint8_t ret = HTTP_OK;
	uint16_t len = 0;
//	uint8_t val = 0;


//	if(predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
//	{
//		;
//	}
//	else 
	
	if(strcmp((const char *)uri_name, "config.cgi") == 0)
	{
		
		//newip		= set_basic_config_setting(p_http_request->URI);
    set_basic_config_setting(p_http_request->URI);
		//Kiem tra xem IP con chuan ko thi moi luu?
		saveip();
		make_cgi_basic_config_response_page(5, device_net_config.ip, buf, &len);
		ret = HTTP_RESET;
	}
	else
	{
		// CGI file not found
		//printf("CGI file not found\r\n");
		ret = HTTP_FAILED;
	}

	if(ret)	*file_len = len;
	return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Get CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void make_json_netinfo(uint8_t * buf, uint16_t * len)
{
	wiz_NetInfo netinfo;
	ctlnetwork(CN_GET_NETINFO, (void*) &netinfo);
	//printf("sec :%d",seconds);
	
//{"mac":"00:08:DC:4F:EB:6E","txtip":"192.168.1.246","gw":"192.168.1.1","txtsn":"255.255.255.1","dns":"8.8.8.8","dhcp":"1","txtdays":"21","txtmonths":"01","txtyears":"2019","txthours":"01","txtminutes":"01","txtseconds":"01","txtgps01":"ON","txtgps02":"OFF","txtpower01":"ON","txtpower02":"OFF"}
	// DHCP: 1 - Static, 2 - DHCP
	if(lostSignal == LOST_GPS_MASTER)
	{
		*len = sprintf((char *)buf, "{\"mac\":\"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\",\"txtip\":\"%d.%d.%d.%d\",\"gw\":\"%d.%d.%d.%d\",\"txtsn\":\"%d.%d.%d.%d\",\"dns\":\"%d.%d.%d.%d\",\"dhcp\":\"%d\",\"txtdays\":\"%d\",\"txtmonths\":\"%d\",\"txtyears\":\"%d\",\"txthours\":\"%d\",\"txtminutes\":\"%d\",\"txtseconds\":\"%d\",\"txtgps01\":\"%s\",\"txtgps02\":\"%s\",\"txtpower01\":\"%s\",\"txtpower02\":\"%s\"}",
											netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5],
											netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3],
											netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3],
											netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3],
											netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3],
											netinfo.dhcp,
											days,
											months,
											years,
											hours,
											minutes,
											seconds,
											gps1_stt?"NO SIGNAL":"NO SIGNAL",
											gps2_stt?"NO SIGNAL":"NO SIGNAL",
											power1_stt?"NO SIGNAL":"NO SIGNAL",
											power2_stt?"NO SIGNAL":"NO SIGNAL"
											);
	}
	else
	{
		*len = sprintf((char *)buf, "{\"mac\":\"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X\",\"txtip\":\"%d.%d.%d.%d\",\"gw\":\"%d.%d.%d.%d\",\"txtsn\":\"%d.%d.%d.%d\",\"dns\":\"%d.%d.%d.%d\",\"dhcp\":\"%d\",\"txtdays\":\"%d\",\"txtmonths\":\"%d\",\"txtyears\":\"%d\",\"txthours\":\"%d\",\"txtminutes\":\"%d\",\"txtseconds\":\"%d\",\"txtgps01\":\"%s\",\"txtgps02\":\"%s\",\"txtpower01\":\"%s\",\"txtpower02\":\"%s\"}",
											netinfo.mac[0], netinfo.mac[1], netinfo.mac[2], netinfo.mac[3], netinfo.mac[4], netinfo.mac[5],
											netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3],
											netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3],
											netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3],
											netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3],
											netinfo.dhcp,
											days,
											months,
											years,
											hours,
											minutes,
											seconds,
											gps1_stt?"ON":"OFF",
											gps2_stt?"ON":"OFF",
											power1_stt?"ON":"OFF",
											power2_stt?"ON":"OFF"
											);
	}		
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Set CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



