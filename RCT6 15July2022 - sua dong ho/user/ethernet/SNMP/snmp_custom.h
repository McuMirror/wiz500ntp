#ifndef _SNMP_CUSTOM_H_
#define _SNMP_CUSTOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#include "snmp.h"

extern dataEntryType snmpData[];
extern const int32_t maxData;


#define COMMUNITY					"public\0"
#define COMMUNITY_SIZE				(strlen(COMMUNITY))

/* Predefined function: Response value control */
void initTable(void);


/* SNMP Trap: warmStart(1) */
void initial_Trap(uint8_t * managerIP, uint8_t * agentIP);

#ifdef __cplusplus
}
#endif

#endif
