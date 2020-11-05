/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_flash.h
 ***********************************************************************/

#pragma once

#include "config.h"

typedef void (*d_flash_spi_cs)(bool state);
typedef uint8_t (*d_flash_spi_read)(bool is_need_more);
typedef void (*d_flash_spi_write)(uint8_t data);

typedef struct {
    d_flash_spi_cs spi_cs;
    d_flash_spi_read spi_read;
    d_flash_spi_write spi_write;
} sSpiFlash_Interface;

// Commands
typedef enum {
    SPI_Flash_Cmd_WriteEnable = 0x06,
    SPI_Flash_Cmd_ReadStatusRegister1 = 0x05,
    SPI_Flash_Cmd_ReadStatusRegister2 = 0x35,
    SPI_Flash_Cmd_PageProgram = 0x02,
    SPI_Flash_Cmd_SectorErase = 0x20,
    SPI_Flash_Cmd_BlockErase = 0x52,
    SPI_Flash_Cmd_ChipErase = 0x60,
    SPI_Flash_Cmd_ReadData = 0x03,
    SPI_Flash_Cmd_ReadJEDEC = 0x9F,
} eSpiFlash_Cmd;

// Status Register 1
typedef enum {
    SPI_Flash_StatusRegister1_Busy = (1 << 0),
    SPI_Flash_StatusRegister1_WEL = (1 << 1),
    SPI_Flash_StatusRegister1_BP = (7 << 2),
    SPI_Flash_StatusRegister1_TB = (1 << 5),
    SPI_Flash_StatusRegister1_SEC = (1 << 6),
    SPI_Flash_StatusRegister1_SRP0 = (1 << 7),
} eSpiFlash_StatusRegister1;

// Status Register 2
typedef enum {
    SPI_Flash_StatusRegister2_SRP1 = (1 << 0),
    SPI_Flash_StatusRegister2_QE = (1 << 1),
    SPI_Flash_StatusRegister2_LB = (7 << 3),
    SPI_Flash_StatusRegister2_CMP = (1 << 6),
    SPI_Flash_StatusRegister2_SUS = (1 << 7),
} eSpiFlash_StatusRegister2;

// Initialize the Flash controller
bool SpiFlash_Init(sSpiFlash_Interface const *spi_flash_interface);

void SpiFlash_WriteEnable();

// Execute full chip erase
void SpiFlash_EraseChip();

// Execute block erase
void SpiFlash_EraseBlock(uint32_t block_number);

// Is memory busy with executing operation
bool SpiFlash_IsBusy();

// Read data from flash
void SpiFlash_ReadData(uint32_t address, void *data, size_t size);

// Writes page data to flash
void SpiFlash_WritePage(uint32_t address, void const *data, size_t size);
