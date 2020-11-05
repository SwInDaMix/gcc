/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_flash.c
 ***********************************************************************/

#include "stdperiph.h"
#include "config.h"
#include "eta_flash.h"

#define SPIFLASH_NORMALIZE_BYTE(x) (~(x))

static sSpiFlash_Interface const *g_spi_flash_interface;

static void _flash_write_address(uint32_t address) {
    uint8_t const *_ptr = ((uint8_t const *)&address) + 2;
    uint8_t _cnt = 3;
    while (_cnt--) g_spi_flash_interface->spi_write(*_ptr--);
}

static uint8_t _flash_read8() {
    return g_spi_flash_interface->spi_read(false);
}

static uint16_t _flash_read16() {
    uint16_t _data16 = 0;
    uint8_t *_ptr = (uint8_t *)&_data16;
    uint8_t _cnt = 2;
    while (_cnt--) *_ptr++ = g_spi_flash_interface->spi_read(_cnt);
    return _data16;
}

static uint32_t _flash_read24() {
    uint32_t _data24 = 0;
    uint8_t *_ptr = (uint8_t *)&_data24;
    uint8_t _cnt = 3;
    while (_cnt--) *_ptr++ = g_spi_flash_interface->spi_read(_cnt);
    return _data24;
}

static uint32_t _flash_read32() {
    uint32_t _data32 = 0;
    uint8_t *_ptr = (uint8_t *)&_data32;
    uint8_t _cnt = 4;
    while (_cnt--) *_ptr++ = g_spi_flash_interface->spi_read(_cnt);
    return _data32;
}

// Initialize the Flash controller
bool SpiFlash_Init(sSpiFlash_Interface const *spi_flash_interface) {
    g_spi_flash_interface = spi_flash_interface;

    g_spi_flash_interface->spi_cs(true);
    g_spi_flash_interface->spi_write(SPI_Flash_Cmd_ReadJEDEC);
    uint32_t id = _flash_read24();
    g_spi_flash_interface->spi_cs(false);
    return SPIFLASH_ID == id;
}

void SpiFlash_WriteEnable() {
    g_spi_flash_interface->spi_cs(true);
    g_spi_flash_interface->spi_write(SPI_Flash_Cmd_WriteEnable);
    g_spi_flash_interface->spi_cs(false);
}

// Execute full chip erase
void SpiFlash_EraseChip() {
    g_spi_flash_interface->spi_cs(true);
    g_spi_flash_interface->spi_write(SPI_Flash_Cmd_ChipErase);
    g_spi_flash_interface->spi_cs(false);
}

// Execute block erase
void SpiFlash_EraseBlock(uint32_t block_number) {
    g_spi_flash_interface->spi_cs(true);
    g_spi_flash_interface->spi_write(SPI_Flash_Cmd_BlockErase);
    _flash_write_address(block_number * SPIFLASH_BLOCK_SIZE);
    g_spi_flash_interface->spi_cs(false);
}

// Is memory busy with executing operation
bool SpiFlash_IsBusy() {
    g_spi_flash_interface->spi_cs(true);
    g_spi_flash_interface->spi_write(SPI_Flash_Cmd_ReadStatusRegister1);
    uint8_t status1 = _flash_read8();
    g_spi_flash_interface->spi_cs(false);
    return (bool)(status1 & SPI_Flash_StatusRegister1_Busy);
}

// Read data from flash
void SpiFlash_ReadData(uint32_t address, void *data, size_t size) {
    uint8_t *_ptr = data;
    g_spi_flash_interface->spi_cs(true);
    g_spi_flash_interface->spi_write(SPI_Flash_Cmd_ReadData);
    _flash_write_address(address);
    while (size--) *_ptr++ = SPIFLASH_NORMALIZE_BYTE(g_spi_flash_interface->spi_read(size));
    g_spi_flash_interface->spi_cs(false);
}

// Writes page data to flash
void SpiFlash_WritePage(uint32_t address, void const *data, size_t size) {
    uint8_t const *_ptr = data;
    g_spi_flash_interface->spi_cs(true);
    g_spi_flash_interface->spi_write(SPI_Flash_Cmd_PageProgram);
    _flash_write_address(address);
    while (size--) g_spi_flash_interface->spi_write(SPIFLASH_NORMALIZE_BYTE(*_ptr++));
    g_spi_flash_interface->spi_cs(false);
}