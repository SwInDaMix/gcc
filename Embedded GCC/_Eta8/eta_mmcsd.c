/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_mmcsd.c
 ***********************************************************************/

#include "eta_mmcsd.h"
#include "eta_dbg.h"
#include <util/delay.h>
#include "util/crc16.h"

#ifdef MMCSD

static bool MMCSD_ReInited;
static uint32_t MMCSD_CurrentSector;
static uint16_t MMCSD_SectorBytes;
static eMMCSDCardType MMCSD_CardType;

static bool MMCSD_WaitNotBusy() {
	uint16_t _retries = 10000;
	while(MMCSD_SPI_ReadWrite(0xFF) != 0xFF && _retries--);
	return _retries;
}
static uint8_t MMCSD_WaitToken() {
	uint16_t _tries = 800; uint8_t _res;
	while((_res = MMCSD_SPI_ReadWrite(0xFF)) == 0xFF && _tries--) _delay_us(100);
	return _res;
}
static uint8_t MMCSD_Cmd(eMMCSDCommand cmd, uint32_t value) {
	uint8_t _res;
	MMCSD_WaitNotBusy();
	if(cmd & 0x80) { MMCSD_Cmd(SDCommand_CMD55, 0); cmd &= 0x7F; }
	MMCSD_SPI_ReadWrite(cmd);
	MMCSD_SPI_ReadWrite(value >> 24);
	MMCSD_SPI_ReadWrite(value >> 16);
	MMCSD_SPI_ReadWrite(value >> 8);
	MMCSD_SPI_ReadWrite(value);
	uint8_t _crc = cmd == SDCommand_CMD0 ? 0x95 : cmd == SDCommand_CMD8 ? 0x87 : 1;
	MMCSD_SPI_ReadWrite(_crc);
	uint8_t _tries = 0xFF;
	while((_res = MMCSD_SPI_ReadWrite(0xFF)) & 0x80 && _tries--);
	return _res;
}
static bool MMCSD_CmdWithWait(eMMCSDCommand cmd, uint32_t value) {
	uint16_t _retries = 10000;
	while(MMCSD_Cmd(cmd, value) && _retries--);
	return _retries;
}
static uint32_t MMCSD_Recieve32() { return make_int32_be(MMCSD_SPI_ReadWrite(0xFF), MMCSD_SPI_ReadWrite(0xFF), MMCSD_SPI_ReadWrite(0xFF), MMCSD_SPI_ReadWrite(0xFF)); }

eMMCSDCardType MMCSD_CardType_get() { return MMCSD_CardType; }
uint32_t MMCSD_CurrentSector_get() { return MMCSD_CurrentSector; }
bool MMCSD_Init() {
	MMCSD_SPI_CSOff();
	_delay_ms(50);
	for(uint8_t _i = 10; _i; _i--) MMCSD_SPI_ReadWrite(0xFF);
	_delay_us(50);
	MMCSD_SPI_CSOn();

	// GO_IDLE_STATE
	uint16_t _tries = 20;
	while(_tries-- && MMCSD_Cmd(SDCommand_CMD0, 0) != 1);
	if(_tries) {
		// Check for SDv2
		if(MMCSD_Cmd(SDCommand_CMD8, 0x1AA) == 1) {
			if((((uint16_t)MMCSD_Recieve32()) & 0xFFF) == 0x1AA) {
				if(MMCSD_CmdWithWait(SDCommand_ACMD41, (1UL << 30))) {
					if(!MMCSD_Cmd(SDCommand_CMD58, 0)) {
						MMCSD_CardType = (MMCSD_Recieve32() & (1UL << 30)) ? SDCardType_SD2 | SDCardType_CT_BLOCK : SDCardType_SD2;
					}
				}
			}
		}
		// Card SDv1 or MMC
		else {
			eMMCSDCommand _cmd_next;
			if(MMCSD_Cmd(SDCommand_ACMD41, 0) <= 1) { MMCSD_CardType = SDCardType_SD1; _cmd_next = SDCommand_ACMD41; }
			else { MMCSD_CardType = SDCardType_MMC; _cmd_next = SDCommand_CMD1; }
			if(!MMCSD_CmdWithWait(_cmd_next, 0) || MMCSD_Cmd(SDCommand_CMD16, 512)) MMCSD_CardType = 0;
		}
	}
	return (bool)MMCSD_CardType;
}
bool MMCSD_ReadInit(uint32_t sector) {
	while(true) {
		if(!MMCSD_Cmd(SDCommand_CMD17, MMCSD_CardType & SDCardType_CT_BLOCK ? sector : sector << 9)) {
			if(MMCSD_WaitToken() == 0xFE) {
				MMCSD_CurrentSector = sector; MMCSD_SectorBytes = 0;
				return true;
			}
		}
		if(!MMCSD_ReInited && MMCSD_Init()) continue;
		MMCSD_ReInited = true; return false;
	}
}
sMMCSDReadResult MMCSD_ReadByte() {
	sMMCSDReadResult _res; _res.result = true;
	if(MMCSD_SectorBytes == 512) { if(!(_res.result = MMCSD_ReadFinalize() && MMCSD_ReadInit(MMCSD_CurrentSector))) return _res; }
	MMCSD_SectorBytes++;
	_res.byte = MMCSD_SPI_ReadWrite(0xFF);
	return _res;
}
bool MMCSD_ReadBlock(void *dst, size_t len) {
	uint8_t *_dst = (uint8_t *)dst;
	while(len--) {
		sMMCSDReadResult _res = MMCSD_ReadByte();
		if(!_res.result) return false;
		*_dst++ = _res.byte;
	}
	return true;
}
bool MMCSD_ReadFinalize() {
	uint16_t _dummy = 512 - MMCSD_SectorBytes;
	while(_dummy--) { MMCSD_SPI_ReadWrite(0xFF); }
	MMCSD_CurrentSector++;
	return true;
}
bool MMCSD_WriteInit(uint32_t sector) {
	while(true) {
		if(!MMCSD_Cmd(SDCommand_CMD24, MMCSD_CardType & SDCardType_CT_BLOCK ? sector : sector << 9)) {
			MMCSD_CurrentSector = sector; MMCSD_SectorBytes = 0;
			MMCSD_SPI_ReadWrite(0xFE);
			return true;
		}
		if(!MMCSD_ReInited && MMCSD_Init()) continue;
		MMCSD_ReInited = true; return false;
	}
}
bool MMCSD_WriteByte(uint8_t byte) {
	if(MMCSD_SectorBytes == 512) { if(!MMCSD_WriteFinalize() || !MMCSD_WriteInit(MMCSD_CurrentSector)) return false; }
	MMCSD_SPI_ReadWrite(byte);
	MMCSD_SectorBytes++;
	return true;
}
bool MMCSD_WriteBlock(void const *src, size_t len) {
	uint8_t const *_src = (uint8_t const *)src;
	while(len--) { if(!MMCSD_WriteByte(*_src++)) return false; }
	return true;
}
bool MMCSD_WriteFinalize() {
	uint16_t _dummy = 514 - MMCSD_SectorBytes;
	while(_dummy--) { MMCSD_SPI_ReadWrite(0x00); }
	MMCSD_CurrentSector++;
	if((MMCSD_SPI_ReadWrite(0xFF) & 0x1F) == 0x05) { return MMCSD_WaitNotBusy(); }
	return false;
}

#endif
