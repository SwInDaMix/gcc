/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.h
 ***********************************************************************/

#pragma once

#include "config.h"

#define CMD_MAX_DATA_SIZE_SHIFT 4
#define CMD_MAX_DATA_SIZE (1 << CMD_MAX_DATA_SIZE_SHIFT)
#define CMD_EEPROM_PAGES (64 / CMD_MAX_DATA_SIZE)

#define ADC_UNLOCKED_VALUE (ADC_MAXVALUE / 3)
#define ADC_LOCKED_VALUE ((ADC_MAXVALUE * 2) / 3)

#define DEFAULT_ADDR 0xAA

typedef enum {
	CMD_StateChanged	= 0x00,
	CMD_Lock			= 0x80,
	CMD_Unlock			= 0x81,
	CMD_GetState		= 0x82,
} eCmd;

typedef enum {
	LockDrive_Idle		= 0,
	LockDrive_Lock		= (1 << SW_LOW_LOCKMOTOR_PIN),
	LockDrive_Unlock	= (1 << SW_HIGH_LOCKMOTOR_PIN),
	LockDrive__Mask		= (1 << SW_LOW_LOCKMOTOR_PIN) | (1 << SW_HIGH_LOCKMOTOR_PIN),
} eLockDrive;

typedef enum {
	LockState_Fail,
	LockState_Unlocked,
	LockState_Locked,
} eLockState;
