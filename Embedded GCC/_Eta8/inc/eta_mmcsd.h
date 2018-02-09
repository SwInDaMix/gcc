/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_mmcsd.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include <avr/io.h>

#ifdef MMCSD

typedef enum {
	SDCommand_CMD0    = (0x40+0),	// ����������� ����� ����� ������ (�����).
	SDCommand_CMD1    = (0x40+1),	// ������ �������� �������������.
	SDCommand_CMD8    = (0x40+8),	// �������� ��������� ���������� �������.
	SDCommand_CMD12   = (0x40+12),	// ���������� ������ ������.
	SDCommand_CMD16   = (0x40+16),	// ��������� ������� ����� R/W (������/������).
	SDCommand_CMD17   = (0x40+17),	// ������ �����.
	SDCommand_CMD18   = (0x40+18),	// ������ ��������� ������ (��������).
	SDCommand_CMD24   = (0x40+24),	// ������ �����.
	SDCommand_CMD25   = (0x40+25),	// ������ ���������� ������.
	SDCommand_CMD55   = (0x40+55),	// ������� ������ ����������� ����� ACMD ���������.
	SDCommand_CMD58   = (0x40+58),	// ������ OCR.

	SDCommand_ACMD41  = (0xC0+41),	// ������ �������� ������������� ����� ���� SDC (microsd).
} eMMCSDCommand;

typedef enum {
	SDCardType_MMC = 0x01,									// MMC ver 3
	SDCardType_SD1 = 0x02,									// SD ver 1
	SDCardType_SD2 = 0x04,									// SD ver 2
	SDCardType_SDC = (SDCardType_SD1 | SDCardType_SD2),		// SD
	SDCardType_CT_BLOCK = 0x08,								// Block addressing
} eMMCSDCardType;

typedef struct {
	uint8_t byte;
	bool result;
} sMMCSDReadResult;

eMMCSDCardType MMCSD_CardType_get();
uint32_t MMCSD_CurrentSector_get();
bool MMCSD_Init();
bool MMCSD_ReadInit(uint32_t sector);
sMMCSDReadResult MMCSD_ReadByte();
bool MMCSD_ReadBlock(void *dst, size_t len);
bool MMCSD_ReadFinalize();
bool MMCSD_WriteInit(uint32_t sector);
bool MMCSD_WriteByte(uint8_t byte);
bool MMCSD_WriteBlock(void const *src, size_t len);
bool MMCSD_WriteFinalize();

#endif
