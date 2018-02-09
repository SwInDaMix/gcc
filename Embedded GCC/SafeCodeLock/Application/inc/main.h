/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.h
 ***********************************************************************/

#pragma once

#include "config.h"

#define SAFE_PASSWORD_LENGTH_MIN	3
#define SAFE_PASSWORD_LENGTH_MAX	10
#define SAFE_PASSWORD_DEFAULT		'1', '2', '3', '4', '5', '6'
#define SAFE_PASSWORD_MASTER		'0', '5', '1', '0', '1', '9', '8', '3', '0', '7'

#define PENALTY_TICKS				30
#define PENALTY_MAX_WRONG_PASSWORDS	5

#define GATE_PWM_DUTY_8BIT			85

#define PERIOD_1S 					20
#define PERIOD_BATTERY_MONITOR_LOAD	2
#define PERIOD_GATE					(2 * PERIOD_1S)
#define PERIOD_POWER_DOWN			(5 * PERIOD_1S)
#define PERIOD_POWER_DOWN_SETTINGS	(12 * PERIOD_1S)
#define PERIOD_PENALTY_ONETICK		(10 * PERIOD_1S)
#define PERIOD_KEY_SHORT			1
#define PERIOD_KEY_LONG				(2 * PERIOD_1S)

#define BEEPCFG_KEYPRESS				0x01	// 00000001
#define BEEPCFG_KEYPRESS_LOW_BATTERY	0x15	// 00010101
#define BEEPCFG_PASSWORD_WRONG			0x1F	// 00011111
#define BEEPCFG_PASSWORD_OK				0x05	// 00000101
#define BEEPCFG_EEPROM_FAILED			0xEF	// 11101111
#define BEEPCFG_PASSWORD_CHANGE			0x55	// 01010101
#define BEEPCFG_PASSWORD_NEW_OK			0x1B	// 00011011
#define BEEPCFG_PASSWORD_CHANGED		0xBD	// 10111101

#define KEYRES_REFERENCE	5100
#define KEYRES_0			0
#define KEYRES_1			523
#define KEYRES_2			953
#define KEYRES_3			1430
#define KEYRES_4			2000
#define KEYRES_5			2670
#define KEYRES_6			3480
#define KEYRES_7			4530
#define KEYRES_8			5760
#define KEYRES_9			7500
#define KEYRES_STAR			10000
#define KEYRES_CHARP		13700

#define KEYVAL_0			(KEYRES_0 * ADC_MAXVALUE / (KEYRES_0 + KEYRES_REFERENCE))
#define KEYVAL_1			((unsigned long long)KEYRES_1 * ADC_MAXVALUE / (KEYRES_1 + KEYRES_REFERENCE))
#define KEYVAL_2			((unsigned long long)KEYRES_2 * ADC_MAXVALUE / (KEYRES_2 + KEYRES_REFERENCE))
#define KEYVAL_3			((unsigned long long)KEYRES_3 * ADC_MAXVALUE / (KEYRES_3 + KEYRES_REFERENCE))
#define KEYVAL_4			((unsigned long long)KEYRES_4 * ADC_MAXVALUE / (KEYRES_4 + KEYRES_REFERENCE))
#define KEYVAL_5			((unsigned long long)KEYRES_5 * ADC_MAXVALUE / (KEYRES_5 + KEYRES_REFERENCE))
#define KEYVAL_6			((unsigned long long)KEYRES_6 * ADC_MAXVALUE / (KEYRES_6 + KEYRES_REFERENCE))
#define KEYVAL_7			((unsigned long long)KEYRES_7 * ADC_MAXVALUE / (KEYRES_7 + KEYRES_REFERENCE))
#define KEYVAL_8			((unsigned long long)KEYRES_8 * ADC_MAXVALUE / (KEYRES_8 + KEYRES_REFERENCE))
#define KEYVAL_9			((unsigned long long)KEYRES_9 * ADC_MAXVALUE / (KEYRES_9 + KEYRES_REFERENCE))
#define KEYVAL_STAR			((unsigned long long)KEYRES_STAR * ADC_MAXVALUE / (KEYRES_STAR + KEYRES_REFERENCE))
#define KEYVAL_CHARP		((unsigned long long)KEYRES_CHARP * ADC_MAXVALUE / (KEYRES_CHARP + KEYRES_REFERENCE))

#define KEYMID_01			((KEYVAL_0 + KEYVAL_1) >> 1)
#define KEYMID_12			((KEYVAL_1 + KEYVAL_2) >> 1)
#define KEYMID_23			((KEYVAL_2 + KEYVAL_3) >> 1)
#define KEYMID_34			((KEYVAL_3 + KEYVAL_4) >> 1)
#define KEYMID_45			((KEYVAL_4 + KEYVAL_5) >> 1)
#define KEYMID_56			((KEYVAL_5 + KEYVAL_6) >> 1)
#define KEYMID_67			((KEYVAL_6 + KEYVAL_7) >> 1)
#define KEYMID_78			((KEYVAL_7 + KEYVAL_8) >> 1)
#define KEYMID_89			((KEYVAL_8 + KEYVAL_9) >> 1)
#define KEYMID_9STAR		((KEYVAL_9 + KEYVAL_STAR) >> 1)
#define KEYMID_STARCHARP	((KEYVAL_STAR + KEYVAL_CHARP) >> 1)
#define KEYMID_CUTOFF		(KEYVAL_CHARP + KEYVAL_CHARP - KEYMID_STARCHARP)

#define BATTERY_RES_BOTTOM			47
#define BATTERY_RES_TOP				150
#define BATTERY_LOW_MILIVOLTAGE		7500
#define BATTERY_ADC_CUTOFF			((unsigned long long)ADC_MAXVALUE * ((unsigned long long)BATTERY_LOW_MILIVOLTAGE * BATTERY_RES_BOTTOM / (BATTERY_RES_BOTTOM + BATTERY_RES_TOP)) / V_CPU)

typedef struct {
	uint8_t PasswordLength;
	char Password[SAFE_PASSWORD_LENGTH_MAX];
} sSettings;

typedef struct {
	uint8_t WrongPasswordsEntered;
	uint8_t PenaltyTicks;
} sPenalties;
