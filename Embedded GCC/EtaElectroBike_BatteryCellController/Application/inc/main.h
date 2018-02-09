/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.h
 ***********************************************************************/

#pragma once

#include "config.h"

#define CMD_MAX_DATA_SIZE_SHIFT 4
#define CMD_MAX_DATA_SIZE (1 << CMD_MAX_DATA_SIZE_SHIFT)
#define CMD_EEPROM_PAGES (64 / CMD_MAX_DATA_SIZE)

#define ADC_REPEAT_SHIFT 4

#define DEFAULT_ADDR 0xB0

#define CMD_SUB_MASK 0x30
#define CMD_SUB_SHIFT 4
#define CMD_CMD_MASK 0x0F
#define CMD_CMD_SHIFT 0

typedef enum {
	CMD_WriteEEPROM		= 0,
	CMD_ReadEEPROM		= 1,
	CMD_GetVoltage		= 2,
	CMD_SwitchSink		= 3,
} eCmd;

typedef enum {
	SINK_Low			= (1 << SW_LOW_SINK_PIN),
	SINK_High			= (1 << SW_HIGH_SINK_PIN),
	SINK__Mask			= (1 << SW_LOW_SINK_PIN) | (1 << SW_HIGH_SINK_PIN),
} eSink;

typedef struct {
	uint8_t Data[CMD_MAX_DATA_SIZE];
} sEEPROM;
