#include <stdio.h>
#include <time.h>
#include <string.h>
#include "GPIO_STM32F10x.h"             // Keil::Device:GPIO
#include "w5500.h"
#include "socket.h"
#include "socketdefines.h"

//#define NTP_PORT                123

#define LEAP_MASK               0xC0
#define LEAP_NONE               0x0
#define LEAP_INSERT             0x1
#define LEAP_DELETE             0x2
#define LEAP_UNKNOWN            0x3
#define VN_MASK                 0x38
#define VN_4                    (4 << 3)
#define MODE_MASK               0x07
#define MODE_CLIENT             0x3
#define MODE_SERVER             0x4


/* Shifts usecs in unixToNtpTime */
//??? ko hieu nhung dung! 
#ifndef USECSHIFT
#define USECSHIFT (1LL << 32) * 1.0e-6
#endif
//Number of seconds from 1st January 1900 to start of Unix epoch
//According to the Time protocol in RFC 868 it is 2208988800L.
#define STARTOFTIME 2208988800UL

#ifndef UTIL_H
#define UTIL_H

#define htons(x) ( ((x)<< 8 & 0xFF00) | \
                   ((x)>> 8 & 0x00FF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)
#endif

extern time_t unixTime_last_sync;// lan chuan gio gan nhat 
/* SNTP Packet array */
extern uint8_t serverPacket[NTP_PACKET_SIZE] ;
extern uint8_t clientPacket[NTP_PACKET_RAWSIZE] ;
extern time_t micros_recv ;
extern time_t micros_offset;
extern time_t transmitTime;
extern time_t micros_transmit;
extern time_t recvTime;

int32_t NTPUDP(uint8_t sn);
void wzn_event_handle(void);
void ntpserverdefaultconfig(void);
int32_t ntpserverprocess(void);

//Tao ban tin NTP
						//ntpserverdefaultconfig();
						/*
						serverPacket[0] = 0x24;   // LI, Version, Mode // Set version number and mode
						serverPacket[1] = 1; // Stratum, or type of clock
						serverPacket[2] = 0;     // Polling Interval
						serverPacket[3] = -12;  // Peer Clock Precision
						serverPacket[12] = 'G';
						serverPacket[13] = 'P';
						serverPacket[14] = 'S';

						
					serverPacket[0]=0x24;	// Leap 0x0, Version 0x3, Mode 0x4
				  serverPacket[1]=0x03;	// Stratum 0x1, stratum (GPS)
				  serverPacket[2]=0x06;	// polling minimum (64 seconds - default)
				  //  serverPacket[3] = 0xFA; // precision (reference sketch - ~15 milliseconds)
					//serverPacket[3] = 0xF7; // precision (2^-9 ~2 milliseconds)
					serverPacket[3] = 0xE8; 
					//  serverPacket[3] = 0x09; // precision (2^9 Testing)
					// root delay
					serverPacket[4]=0x0;
					serverPacket[5]=0x0;
					serverPacket[6]=0x2C;
					serverPacket[7]=0x3C;
					
					serverPacket[8]=0x0;
					serverPacket[9]=0x0;
					serverPacket[10]=0x0E;
					serverPacket[11] = 0x7D; // root dispersion
					
					serverPacket[12]=0x47;	// Reference ID, "G"
					serverPacket[13]=0x50;	// Reference ID, "P"
					serverPacket[14]=0x53;	// Reference ID, "S"
					serverPacket[15]=0x00;	// Reference ID, 0x00

						*/