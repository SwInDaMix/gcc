/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_ade7753.c
 ***********************************************************************/

#include "config.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "eta_ade7753.h"

#ifdef ADE7753

// Inits all ADE7753
void ADE7753_Init_ISR(sADE7753_Config const *config, sADE7753_Calibration const *calibration) {
    ADE7753_Write1_ISR(ADE7753Register_MODE, ADE7753Mode_SWRST);
    _delay_us(100);
	ADE7753_Write2_ISR(ADE7753Register_MODE,  pgm_read_word_near(&config->MODE));
	ADE7753_Write2_ISR(ADE7753Register_IRQEN, pgm_read_word_near(&config->IRQEN));
	ADE7753_Write1_ISR(ADE7753Register_GAIN, pgm_read_byte_near(&config->GAIN));
	ADE7753_Write2_ISR(ADE7753Register_LINECYC, pgm_read_word_near(&config->LINECYC));
	ADE7753_Write2_ISR(ADE7753Register_ZXTOUT, pgm_read_word_near(&config->ZXTOUT));
	ADE7753_Write1_ISR(ADE7753Register_SAGCYC, pgm_read_byte_near(&config->SAGCYC));

	ADE7753_InitCalibration_ISR(calibration);
}
void ADE7753_Init(sADE7753_Config const *config, sADE7753_Calibration const *calibration) { cli(); ADE7753_Init_ISR(config, calibration); sei(); }

// Inits calibration parameters ADE7753
void ADE7753_InitCalibration_ISR(sADE7753_Calibration const *calibration) {
	ADE7753_Write1_ISR(ADE7753Register_CH1OS, calibration->CH1OS);
	ADE7753_Write1_ISR(ADE7753Register_CH2OS, calibration->CH2OS);
	ADE7753_Write1_ISR(ADE7753Register_PHCAL, calibration->PHCAL);
	ADE7753_Write2_ISR(ADE7753Register_APOS, calibration->APOS);
	ADE7753_Write2_ISR(ADE7753Register_WGAIN, calibration->WGAIN);
	ADE7753_Write1_ISR(ADE7753Register_WDIV, calibration->WDIV);
	ADE7753_Write2_ISR(ADE7753Register_CFNUM, calibration->CFNUM);
	ADE7753_Write2_ISR(ADE7753Register_CFDEN, calibration->CFDEN);
	ADE7753_Write2_ISR(ADE7753Register_IRMSOS, calibration->IRMSOS);
	ADE7753_Write2_ISR(ADE7753Register_VRMSOS, calibration->VRMSOS);
	ADE7753_Write2_ISR(ADE7753Register_VAGAIN, calibration->VAGAIN);
	ADE7753_Write1_ISR(ADE7753Register_VADIV, calibration->VADIV);
	ADE7753_Write1_ISR(ADE7753Register_SAGLVL, calibration->SAGLVL);
	ADE7753_Write1_ISR(ADE7753Register_IPKLVL, calibration->IPKLVL);
	ADE7753_Write1_ISR(ADE7753Register_VPKLVL, calibration->VPKLVL);
}
void ADE7753_InitCalibration(sADE7753_Calibration const *calibration) { cli(); ADE7753_InitCalibration_ISR(calibration); sei(); }

// Writes a byte register to ADE7753
void ADE7753_Write1_ISR(eADE7753_Register reg, uint8_t data) {
	ADE7753_SPI_CSOn();
	ADE7753_SPI_ReadWrite(reg | 0x80);
	ADE7753_SPI_ReadWrite(data);
	ADE7753_SPI_CSOff();
}
void ADE7753_Write1(eADE7753_Register reg, uint8_t data) { cli(); ADE7753_Write1_ISR(reg, data); sei(); }

// Writes a 2 byte register to ADE7753
void ADE7753_Write2_ISR(eADE7753_Register reg, uint16_t data) {
	ADE7753_SPI_CSOn();
	ADE7753_SPI_ReadWrite(reg | 0x80);
	ADE7753_SPI_ReadWrite((uint8_t)(data >> 8));
	ADE7753_SPI_ReadWrite((uint8_t)data);
	ADE7753_SPI_CSOff();
}
void ADE7753_Write2(eADE7753_Register reg, uint16_t data) { cli(); ADE7753_Write2_ISR(reg, data); sei(); }

// Retrieve a byte register from ADE7753
uint8_t ADE7753_Read1_ISR(eADE7753_Register reg) {
	uint8_t _data;
	ADE7753_SPI_CSOn();
	ADE7753_SPI_ReadWrite(reg & 0x7F);
	_delay_us(4);
	_data = ADE7753_SPI_ReadWrite(0);
	ADE7753_SPI_CSOff();
	return _data;
}
uint8_t ADE7753_Read1(eADE7753_Register reg) { cli(); uint8_t _res = ADE7753_Read1_ISR(reg); sei(); return _res; }

// Retrieve a 2 byte register from ADE7753
uint16_t ADE7753_Read2_ISR(eADE7753_Register reg) {
	uint16_t _data;
	ADE7753_SPI_CSOn();
	ADE7753_SPI_ReadWrite(reg & 0x7F);
	_delay_us(4);
	_data = ADE7753_SPI_ReadWrite(0) << 8;
	_data |= ADE7753_SPI_ReadWrite(0);
	ADE7753_SPI_CSOff();
	return _data;
}
uint16_t ADE7753_Read2(eADE7753_Register reg) { cli(); uint16_t _res = ADE7753_Read2_ISR(reg); sei(); return _res; }

// Retrieve a 3 byte register from ADE7753
uint24_t ADE7753_Read3_ISR(eADE7753_Register reg) {
	uint24_t _data;
	ADE7753_SPI_CSOn();
	ADE7753_SPI_ReadWrite(reg & 0x7F);
	_delay_us(4);
	_data = ((uint24_t)ADE7753_SPI_ReadWrite(0)) << 16;
	_data |= ((uint16_t)ADE7753_SPI_ReadWrite(0)) << 8;
	_data |= ADE7753_SPI_ReadWrite(0);
	ADE7753_SPI_CSOff();
	return _data;
}
uint24_t ADE7753_Read3(eADE7753_Register reg) { cli(); uint24_t _res = ADE7753_Read3_ISR(reg); sei(); return _res; }

// Retrieve a 3 byte register from ADE7753 (fast reading for waveform samples)
uint24_t ADE7753_ReadFast3_ISR(eADE7753_Register reg) {
	uint24_t _data;
	ADE7753_SPI_CSOn();
	ADE7753_SPI_ReadWrite(reg & 0x7F);
	ADE7753_DELAY();
	_data = ((uint24_t)ADE7753_SPI_ReadWrite(0)) << 16;
	_data |= ((uint16_t)ADE7753_SPI_ReadWrite(0)) << 8;
	_data |= ADE7753_SPI_ReadWrite(0);
	ADE7753_SPI_CSOff();
	return _data;
}
uint24_t ADE7753_ReadFast3(eADE7753_Register reg) { cli(); uint24_t _res = ADE7753_ReadFast3_ISR(reg); sei(); return _res; }
#endif
