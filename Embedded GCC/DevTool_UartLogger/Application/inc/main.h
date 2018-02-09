/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.h
 ***********************************************************************/

#pragma once

#include "config.h"

#define TICKS_PER_SECOND (F_CPU / 64 / 256)

#define MEMORY_BLOCK_SIZE 32

typedef enum {
	SDCommand_CMD0    = (0x40+0),	// Программный сброс карты памяти (любой).
	SDCommand_CMD1    = (0x40+1),	// Запуск процесса инициализации.
	SDCommand_CMD8    = (0x40+8),	// Проверка диапазона напряжения питания.
	SDCommand_CMD12   = (0x40+12),	// Прекратить чтение данных.
	SDCommand_CMD16   = (0x40+16),	// Изменение размера блока R/W (чтения/записи).
	SDCommand_CMD17   = (0x40+17),	// Чтение блока.
	SDCommand_CMD18   = (0x40+18),	// Чтение множества блоков (секторов).
	SDCommand_CMD24   = (0x40+24),	// Запись блока.
	SDCommand_CMD25   = (0x40+25),	// Запись нескольких блоков.
	SDCommand_CMD55   = (0x40+55),	// Команда должна посыллаться перед ACMD командами.
	SDCommand_CMD58   = (0x40+58),	// Чтение OCR.

	SDCommand_ACMD41  = (0xC0+41),	// Запуск процесса инициализации карты типа SDC (microsd).
} eUL_SDCommand;

typedef enum {
	SDCardType_MMC = 0x01,									// MMC ver 3
	SDCardType_SD1 = 0x02,									// SD ver 1
	SDCardType_SD2 = 0x04,									// SD ver 2
	SDCardType_SDC = (SDCardType_SD1 | SDCardType_SD2),		// SD
	SDCardType_CT_BLOCK = 0x08,								// Block addressing
} eUL_SDCardType;
