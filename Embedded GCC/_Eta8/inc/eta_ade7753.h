/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_ade7753.h
 ***********************************************************************/

#pragma once

#include "config.h"
#include <avr/io.h>

#ifdef ADE7753
#define ADE7753_FULLSACLE_I_SAMPLING 0x2851EC
#define ADE7753_FULLSACLE_V_SAMPLING 0x2852
#define ADE7753_FULLSACLE_IRMS 0x1C82B3
#define ADE7753_FULLSACLE_VRMS 0x17D338

// ADE7753 Registers indeces
typedef enum {
	ADE7753Register_WAVEFORM = 0x01,		// R	24 0x0		S Waveform Register. This read-only register contains the sampled waveform data from either Channel 1, Channel 2, or the active power signal. The data source and the length of the waveform registers are selected by data Bits 14 and 13 in the mode register—see the Channel 1 Sampling and Channel 2 Sampling sections.
	ADE7753Register_AENERGY = 0x02,			// R	24 0x0		S Active Energy Register. Active power is accumulated (integrated) over time in this 24-bit, read-only register—see the Energy Calculation section.
	ADE7753Register_RAENERGY = 0x03,		// R	24 0x0		S Same as the active energy register except that the register is reset to 0 following a read operation.
	ADE7753Register_LAENERGY = 0x04,		// R	24 0x0		S Line Accumulation Active Energy Register. The instantaneous active power is accumulated in this read-only register over the LINECYC number of half line cycles.
	ADE7753Register_VAENERGY = 0x05,		// R	24 0x0		U Apparent Energy Register. Apparent power is accumulated over time in this read-only register.
	ADE7753Register_RVAENERGY = 0x06,		// R	24 0x0		U Same as the VAENERGY register except that the register is reset to 0 following a read operation.
	ADE7753Register_LVAENERGY = 0x07,		// R	24 0x0		U Line Accumulation Apparent Energy Register. The instantaneous real power is accumulated in this read-only register over the LINECYC number of half line cycles.
	ADE7753Register_LVARENERGY = 0x08,		// R	24 0x0		S Line Accumulation Reactive Energy Register. The instantaneous reactive power is accumulated in this read-only register over the LINECYC number of half line cycles.
	ADE7753Register_MODE = 0x09,			// R/W	16 0x000C	U Mode Register. This is a 16-bit register through which most of the ADE7753 functionality is accessed. Signal sample rates, filter enabling, and calibration modes are selected by writing to this register. The contents can be read at any time—see the Mode Register (0x9) section.
	ADE7753Register_IRQEN = 0x0A,			// R/W	16 0x40		U Interrupt Enable Register. ADE7753 interrupts can be deactivated at any time by setting the corresponding bit in this 16- bit enable register to Logic 0. The status register continues to register an interrupt event even if disabled. However, the IRQ output is not activated—see the ADE7753 Interrupts section.
	ADE7753Register_STATUS = 0x0B,			// R	16 0x0		U Interrupt Status Register. This is an 16-bit read-only register. The status register contains information regarding the source of ADE7753 interrupts—the see ADE7753 Interrupts section.
	ADE7753Register_RSTSTATUS = 0x0C,		// R	16 0x0		U Same as the interrupt status register except that the register contents are reset to 0 (all flags cleared) after a read operation.
	ADE7753Register_CH1OS = 0x0D,			// R/W	8  0x0		S* Channel 1 Offset Adjust. Bit 6 is not used. Writing to Bits 0 to 5 allows offsets on Channel 1 to be removed—see the Analog Inputs and CH1OS Register (0x0D) sections. Writing a Logic 1 to the MSB of this register enables the digital integrator on Channel 1, a Logic 0 disables the integrator. The default value of this bit is 0.
	ADE7753Register_CH2OS = 0x0E,			// R/W	8  0x0		S* Channel 2 Offset Adjust. Bits 6 and 7 are not used. Writing to Bits 0 to 5 of this register allows any offsets on Channel 2 to be removed—see the Analog Inputs section. Note that the CH2OS register is inverted. To apply a positive offset, a negative number is written to this register.
	ADE7753Register_GAIN = 0x0F,			// R/W	8  0x0		U PGA Gain Adjust. This 8-bit register is used to adjust the gain selection for the PGA in Channels 1 and 2—see the Analog Inputs section.
	ADE7753Register_PHCAL = 0x10,			// R/W	6  0x0D		S Phase Calibration Register. The phase relationship between Channel 1 and 2 can be adjusted by writing to this 6-bit register. The valid content of this twos compliment register is between 0x1D to 0x21. At a line frequency of 60 Hz, this is a range from –2.06° to +0.7°—see the Phase Compensation section.
	ADE7753Register_APOS = 0x11,			// R/W	16 0x0		S Active Power Offset Correction. This 16-bit register allows small offsets in the active power calculation to be removed—see the Active Power Calculation section.
	ADE7753Register_WGAIN = 0x12,			// R/W	12 0x0		S Power Gain Adjust. This is a 12-bit register. The active power calculation can be calibrated by writing to this register. The calibration range is ±50% of the nominal full-scale active power. The resolution of the gain adjust is 0.0244%/LSB —see the Calibrating an Energy Meter Based on the ADE7753 section.
	ADE7753Register_WDIV = 0x13,			// R/W	8  0x0		U Active Energy Divider Register. The internal active energy register is divided by the value of this register before being stored in the AENERGY register.
	ADE7753Register_CFNUM = 0x14,			// R/W	12 0x3F		U CF Frequency Divider Numerator Register. The output frequency on the CF pin is adjusted by writing to this 12-bit read/write register—see the Energyto-Frequency Conversion section.
	ADE7753Register_CFDEN = 0x15,			// R/W	12 0x3F		U CF Frequency Divider Denominator Register. The output frequency on the CF pin is adjusted by writing to this 12-bit read/write register—see the Energy-to-Frequency Conversion section.
	ADE7753Register_IRMS = 0x16,			// R	24 0x0		U Channel 1 RMS Value (Current Channel).
	ADE7753Register_VRMS = 0x17,			// R	24 0x0		U Channel 2 RMS Value (Voltage Channel).
	ADE7753Register_IRMSOS = 0x18,			// R/W	12 0x0		S Channel 1 RMS Offset Correction Register.
	ADE7753Register_VRMSOS = 0x19,			// R/W	12 0x0		S Channel 2 RMS Offset Correction Register.
	ADE7753Register_VAGAIN = 0x1A,			// R/W	12 0x0		S Apparent Gain Register. Apparent power calculation can be calibrated by writing to this register. The calibration range is 50% of the nominal fullscale real power. The resolution of the gain adjust is 0.02444%/LSB.
	ADE7753Register_VADIV = 0x1B,			// R/W	8  0x0		U Apparent Energy Divider Register. The internal apparent energy register is divided by the value of this register before being stored in the VAENERGY register.
	ADE7753Register_LINECYC = 0x1C,			// R/W	16 0xFFFF	U Line Cycle Energy Accumulation Mode Line-Cycle Register. This 16-bit register is used during line cycle energy accumulation mode to set the number of half line cycles for energy accumulation—see the Line Cycle Energy Accumulation Mode section.
	ADE7753Register_ZXTOUT = 0x1D,			// R/W	12 0xFFF	U Zero-Crossing Timeout. If no zero crossings are detected on Channel 2 within a time period specified by this 12-bit register, the interrupt request line (IRQ) is activated—see the Zero-Crossing Detection section.
	ADE7753Register_SAGCYC = 0x1E,			// R/W	8  0xFF		U Sag Line Cycle Register. This 8-bit register specifies the number of consecutive line cycles the signal on Channel 2 must be below SAGLVL before the SAG output is activated—see the Line Voltage Sag Detection section.
	ADE7753Register_SAGLVL = 0x1F,			// R/W	8  0x0		U Sag Voltage Level. An 8-bit write to this register determines at what peak signal level on Channel 2 the SAG pin becomes active. The signal must remain low for the number of cycles specified in the SAGCYC register before the SAG pin is activated—see the Line Voltage Sag Detection section.
	ADE7753Register_IPKLVL = 0x20,			// R/W	8  0xFF		U Channel 1 Peak Level Threshold (Current Channel). This register sets the level of the current peak detection. If the Channel 1 input exceeds this level, the PKI flag in the status register is set.
	ADE7753Register_VPKLVL = 0x21,			// R/W	8  0xFF		U Channel 2 Peak Level Threshold (Voltage Channel). This register sets the level of the voltage peak detection. If the Channel 2 input exceeds this level, the PKV flag in the status register is set.
	ADE7753Register_IPEAK = 0x22,			// R	24 0x0		U Channel 1 Peak Register. The maximum input value of the current channel since the last read of the register is stored in this register.
	ADE7753Register_RSTIPEAK = 0x23,		// R	24 0x0		U Same as Channel 1 Peak Register except that the register contents are reset to 0 after read.
	ADE7753Register_VPEAK = 0x24,			// R	24 0x0		U Channel 2 Peak Register. The maximum input value of the voltage channel since the last read of the register is stored in this register.
	ADE7753Register_RSTVPEAK = 0x25,		// R	24 0x0		U Same as Channel 2 Peak Register except that the register contents are reset to 0 after a read.
	ADE7753Register_TEMP = 0x26,			// R	8  0x0		S Temperature Register. This is an 8-bit register which contains the result of the latest temperature conversion—see the Temperature Measurement section.
	ADE7753Register_PERIOD = 0x27,			// R	16 0x0		U Period of the Channel 2 (Voltage Channel) Input Estimated by ZeroCrossing Processing. The MSB of this register is always zero.
	ADE7753Register_TMODE = 0x3D,			// R/W	8  – 		U Test Mode Register.
	ADE7753Register_CHKSUM = 0x3E,			// R	6  0x0		U Checksum Register. This 6-bit read-only register is equal to the sum of all the ones in the previous read—see the ADE7753 Serial Read Operation section.
	ADE7753Register_DIEREV = 0x3F,			// R	8 –			U Die Revision Register. This 8-bit read-only register contains the revision number of the silicon.
} eADE7753_Register;

// ADE7753 mode register flags (default value is 0x000C)
typedef enum {
	ADE7753Mode_DISHPF = (1 << 0),				// 0	HPF (high-pass filter) in Channel 1 is disabled when this bit is set.
	ADE7753Mode_DISLPF2 = (1 << 1),				// 0	LPF (low-pass filter) after the multiplier (LPF2) is disabled when this bit is set.
	ADE7753Mode_DISCF = (1 << 2),				// 1	Frequency output CF is disabled when this bit is set.
	ADE7753Mode_DISSAG = (1 << 3),				// 1	Line voltage sag detection is disabled when this bit is set.
	ADE7753Mode_ASUSPEND = (1 << 4),			// 0	By setting this bit to Logic 1, both ADE7753 A/D converters can be turned off. In normal operation, this bit should be left at Logic 0. All digital functionality can be stopped by suspending the clock signal at CLKIN pin.
	ADE7753Mode_TEMPSEL = (1 << 5),				// 0	Temperature conversion starts when this bit is set to 1. This bit is automatically reset to 0 when the temperature conversion is finished.
	ADE7753Mode_SWRST = (1 << 6),				// 0	Software Chip Reset. A data transfer should not take place to the ADE7753 for at least 18 us after a software reset.
	ADE7753Mode_CYCMODE = (1 << 7),				// 0	Setting this bit to Logic 1 places the chip into line cycle energy accumulation mode.
	ADE7753Mode_DISCH1 = (1 << 8),				// 0	ADC 1 (Channel 1) inputs are internally shorted together.
	ADE7753Mode_DISCH2 = (1 << 8),				// 0	ADC 2 (Channel 2) inputs are internally shorted together.
	ADE7753Mode_SWAP = (1 << 10),				// 0	By setting this bit to Logic 1 the analog inputs V2P and V2N are connected to ADC 1 and the analog inputs V1P and V1N are connected to ADC 2.
	ADE7753Mode_DTRT_128 = (0 << 11),			// 00	These bits are used to select the waveform register update rate. 0 27.9 kSPS (CLKIN/128)
	ADE7753Mode_DTRT_256 = (1 << 11),			// 00	These bits are used to select the waveform register update rate. 1 14 kSPS (CLKIN/256)
	ADE7753Mode_DTRT_512 = (2 << 11),			// 00	These bits are used to select the waveform register update rate. 2 7 kSPS (CLKIN/512)
	ADE7753Mode_DTRT_1024 = (3 << 11),			// 00	These bits are used to select the waveform register update rate. 3 3.5 kSPS (CLKIN/1024)
	ADE7753Mode_DTRT__SHIFT = 11,
	ADE7753Mode_DTRT__MASK = (3 << 11),
	ADE7753Mode_WAVSEL_ACTPWR = (0 << 13),		// 00	These bits are used to select the source of the sampled data for the waveform register. 0 24 bits active power signal (output of LPF2)
	ADE7753Mode_WAVSEL_CH1 = (2 << 13),			// 00	These bits are used to select the source of the sampled data for the waveform register. 2 24 bits Channel 1
	ADE7753Mode_WAVSEL_CH2 = (3 << 13),			// 00	These bits are used to select the source of the sampled data for the waveform register. 3 24 bits Channel 2
	ADE7753Mode_WAVSEL__SHIFT = 13,
	ADE7753Mode_WAVSEL__MASK = (3 << 13),
	ADE7753Mode_POAM = (1 << 15),				// 0	Writing Logic 1 to this bit allows only positive active power to be accumulated in the ADE7753.
} eADE7753_Mode;

// ADE7753 interrupts register flags (IRQEN, STATUS, RSTSTATUS)
typedef enum {
	ADE7753Interrupt_AEHF = (1 << 0),		// Indicates that an interrupt occurred because the active energy register, AENERGY, is more than half full.
	ADE7753Interrupt_SAG = (1 << 1),		// Indicates that an interrupt was caused by a SAG on the line voltage.
	ADE7753Interrupt_CYCEND = (1 << 2),		// Indicates the end of energy accumulation over an integer number of half line cycles as defined by the content of the LINECYC register—see the Line Cycle Energy Accumulation Mode section.
	ADE7753Interrupt_WSMP = (1 << 3),		// Indicates that new data is present in the waveform register.
	ADE7753Interrupt_ZX = (1 << 4),			// This status bit is set to Logic 0 on the rising and falling edge of the the voltage waveform. See the Zero-Crossing Detection section.
	ADE7753Interrupt_TEMP = (1 << 5),		// Indicates that a temperature conversion result is available in the temperature register.
	ADE7753Interrupt_RESET = (1 << 6),		// Indicates the end of a reset (for both software or hardware reset). The corresponding enable bit has no function in the interrupt enable register, i.e., this status bit is set at the end of a reset, but it cannot be enabled to cause an interrupt.
	ADE7753Interrupt_AEOF = (1 << 7),		// Indicates that the active energy register has overflowed.
	ADE7753Interrupt_PKV = (1 << 8),		// Indicates that waveform sample from Channel 2 has exceeded the VPKLVL value.
	ADE7753Interrupt_PKI = (1 << 9),		// Indicates that waveform sample from Channel 1 has exceeded the IPKLVL value.
	ADE7753Interrupt_VAEHF = (1 << 10),		// Indicates that an interrupt occurred because the active energy register, VAENERGY, is more than half full.
	ADE7753Interrupt_VAEOF = (1 << 11),		// Indicates that the apparent energy register has overflowed.
	ADE7753Interrupt_ZXTO = (1 << 12),		// Indicates that an interrupt was caused by a missing zero crossing on the line voltage for the specified number of line cycles—see the Zero-Crossing Timeout section.
	ADE7753Interrupt_PPOS = (1 << 13),		// Indicates that the power has gone from negative to positive.
	ADE7753Interrupt_PNEG = (1 << 14),		// Indicates that the power has gone from positive to negative.
} eADE7753_Interrupt;

// ADE7753 Gain register
typedef enum {
	ADE7753Gain_I_PGA_1 = (0 << 0),
	ADE7753Gain_I_PGA_2 = (1 << 0),
	ADE7753Gain_I_PGA_4 = (2 << 0),
	ADE7753Gain_I_PGA_8 = (3 << 0),
	ADE7753Gain_I_PGA_16 = (4 << 0),
	ADE7753Gain_i_FullScale_500 = (0 << 3),
	ADE7753Gain_i_FullScale_250 = (1 << 3),
	ADE7753Gain_i_FullScale_125 = (2 << 3),
	ADE7753Gain_V_PGA_1 = (0 << 5),
	ADE7753Gain_V_PGA_2 = (1 << 5),
	ADE7753Gain_V_PGA_4 = (2 << 5),
	ADE7753Gain_V_PGA_8 = (3 << 5),
	ADE7753Gain_V_PGA_16 = (4 << 5),
} eADE7753_Gain;

// ADE7753 calibration structure
typedef struct {
	uint8_t CH1OS;				// 8
	uint8_t CH2OS;   			// 8
	uint8_t PHCAL;   			// 8
	uint16_t APOS;   			// 16
	uint16_t WGAIN;  			// 16
	uint8_t WDIV;    			// 8
	uint16_t CFNUM;  			// 16
	uint16_t CFDEN;  			// 16
	uint16_t IRMSOS; 			// 16
	uint16_t VRMSOS; 			// 16
	uint16_t VAGAIN; 			// 16
	uint8_t VADIV;   			// 8
	uint8_t SAGLVL;  			// 8
	uint8_t IPKLVL;  			// 8
	uint8_t VPKLVL;  			// 8
} sADE7753_Calibration;

// ADE7753 configuration structure
typedef struct {
	eADE7753_Mode MODE;			// 16
	eADE7753_Interrupt IRQEN;	// 16
	eADE7753_Gain GAIN;    		// 8
	uint16_t LINECYC;			// 16
	uint16_t ZXTOUT; 			// 16
	uint8_t SAGCYC;  			// 8
} sADE7753_Config;

// Inits ADE7753
void ADE7753_Init_ISR(sADE7753_Config const *config, sADE7753_Calibration const *calibration);
void ADE7753_Init(sADE7753_Config const *config, sADE7753_Calibration const *calibration);
// Inits calibration parameters ADE7753
void ADE7753_InitCalibration_ISR(sADE7753_Calibration const *calibration);
void ADE7753_InitCalibration(sADE7753_Calibration const *calibration);
// Writes a byte register to ADE7753
void ADE7753_Write1_ISR(eADE7753_Register reg, uint8_t data);
void ADE7753_Write1(eADE7753_Register reg, uint8_t data);
// Writes a 2 byte register to ADE7753
void ADE7753_Write2_ISR(eADE7753_Register reg, uint16_t data);
void ADE7753_Write2(eADE7753_Register reg, uint16_t data);
// Retrieve a byte register from ADE7753
uint8_t ADE7753_Read1_ISR(eADE7753_Register reg);
uint8_t ADE7753_Read1(eADE7753_Register reg);
// Retrieve a 2 byte register from ADE7753
uint16_t ADE7753_Read2_ISR(eADE7753_Register reg);
uint16_t ADE7753_Read2(eADE7753_Register reg);
// Retrieve a 3 byte register from ADE7753
uint24_t ADE7753_Read3_ISR(eADE7753_Register reg);
uint24_t ADE7753_Read3(eADE7753_Register reg);
// Retrieve a 3 byte register from ADE7753 (fast reading for waveform samples)
uint24_t ADE7753_ReadFast3_ISR(eADE7753_Register reg);
uint24_t ADE7753_ReadFast3(eADE7753_Register reg);

#endif
