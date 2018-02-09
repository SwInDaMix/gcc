/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_rf.h
 ***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined RF_RFM70 ||  defined RF_RFM73 ||  defined RF_RFM75
#define RF_RFM7X
#endif

typedef enum {
	RFCommand_R_REGISTER			= 0x00,	// Read command and status registers. Last 5 bit Register Map Address
	RFCommand_W_REGISTER			= 0x20,	// Write command and status registers. Last 5 bit Register Map Address. Executable in power down or standby modes only
	RFCommand_R_RX_PAYLOAD			= 0x61,	// Read RX-payload: 1 – 32 bytes. A read operation always starts at byte 0. Payload is deleted from FIFO after it is read. Used in RX mode
	RFCommand_W_TX_PAYLOAD			= 0xA0,	// Write TX-payload:  1 – 32 bytes. A write operation always starts at byte 0 used in TX payload
	RFCommand_FLUSH_TX				= 0xE1,	// Flush TX FIFO, used in TX mode
	RFCommand_FLUSH_RX				= 0xE2,	// Flush RX FIFO, used in RX mode. Should not be executed during transmission of acknowledge, that is, acknowledge package will not be completed
	RFCommand_REUSE_TX_PL			= 0xE3,	// Used for a PTX device. Reuse last transmitted payload. Packets are repeatedly retransmitted as long as CE is high. TX payload reuse is active until W_TX_PAYLOAD or FLUSH TX is executed. TX payload reuse must not be activated or deactivated during package transmission
	#if defined RF_RFM7X || defined RF_SE8R01
	RFCommand_ACTIVATE				= 0x50,	// This write command followed by data 0x73 activates the following features: R_RX_PL_WID, W_ACK_PAYLOAD, W_TX_PAYLOAD_NOACK
											// A new ACTIVATE command with the same data deactivates them again. This is executable in power down or stand by modes only
											// The R_RX_PL_WID,  W_ACK_PAYLOAD, and W_TX_PAYLOAD_NOACK features registers are initially in a deactivated state; a write has no effect, a read only results in zeros on MISO. To activate these registers, use the ACTIVATE command followed by data 0x73. Then they can be accessed as any other register. Use the same command and data to deactivate the registers again
											// This write command followed by data 0x53 toggles the register bank, and the current register bank number can be read out from REG7 [7]
	#endif
	RFCommand_R_RX_PL_WID			= 0x60,	// Read RX-payload width for the top R_RX_PAYLOAD in the RX FIFO
	RFCommand_W_ACK_PAYLOAD			= 0xA8,	// Used in RX mode. Write Payload to be transmitted together with ACK packet on PIPE PPP. (PPP valid in the range from 000 to 101). Maximum three ACK packet payloads can be pending. Payloads with same PPP are handled using first in - first out principle. Write payload: 1– 32 bytes. A write operation always starts at byte 0
	RFCommand_W_TX_PAYLOAD_NO_ACK	= 0xB0,	// Used in TX mode. Disables AUTOACK on this specific packet
	RFCommand_NOP					= 0xFF,	// No Operation. Might be used to read the STATUS register
} eRF_Command;

typedef enum {
	RFReg_CONFIG       			= 0x00,		// Configuration register
	RFReg_ENAA         			= 0x01,		// Enable "Auto Acknowledgment" function register
	RFReg_EN_RX_ADDR   			= 0x02,		// Enabled RX addresses register
	RFReg_SETUP_AW     			= 0x03,		// Setup of address widths (common for all data pipes) register
	RFReg_SETUP_RETR   			= 0x04,		// Setup of automatic retransmission register
	RFReg_RF_CH        			= 0x05,		// RF channel register
	RFReg_RF_SETUP     			= 0x06,		// RF setup register
	RFReg_STATUS       			= 0x07,		// Status register (In parallel to the SPI command word applied on the MOSI pin, the STATUS register is shifted serially out on the MISO pin)
	RFReg_OBSERVE_TX   			= 0x08,		// Transmit observe register
	#ifdef RF_SE8R01
	RFReg_RPD           		= 0x09,		// Estimated in-band signal level in dBm, should support -100 ~ +10 dBm
	#else
	RFReg_CD           			= 0x09,		// Carrier detect register
	#endif
	RFReg_RX_ADDR_P0   			= 0x0A,		// Receive address data pipe 0 register. 5 Bytes maximum length, default is 0xE7E7E7E7E7. LSB byte is written first. Write the number of bytes defined by SETUP_AW)
	RFReg_RX_ADDR_P1   			= 0x0B,		// Receive address data pipe 1 register. 5 Bytes maximum length, default is 0xC2C2C2C2C2. LSB byte is written first. Write the number of bytes defined by SETUP_AW)
	RFReg_RX_ADDR_P2   			= 0x0C,		// Receive address data pipe 2 register. 1 byte maximum, default is 0xC3. Only LSB, MSB bytes is equal to RX_ADDR_P1[39:8]
	RFReg_RX_ADDR_P3   			= 0x0D,		// Receive address data pipe 3 register. 1 byte maximum, default is 0xC3. Only LSB, MSB bytes is equal to RX_ADDR_P1[39:8]
	RFReg_RX_ADDR_P4   			= 0x0E,		// Receive address data pipe 4 register. 1 byte maximum, default is 0xC3. Only LSB, MSB bytes is equal to RX_ADDR_P1[39:8]
	RFReg_RX_ADDR_P5   			= 0x0F,		// Receive address data pipe 5 register. 1 byte maximum, default is 0xC3. Only LSB, MSB bytes is equal to RX_ADDR_P1[39:8]
	RFReg_TX_ADDR      			= 0x10,		// Transmit address register
	RFReg_RX_PW_P0     			= 0x11,		// RX payload width register: number of bytes in RX payload in data pipe 0 (1 to 32 bytes), 0 is default
	RFReg_RX_PW_P1     			= 0x12,		// RX payload width register: number of bytes in RX payload in data pipe 1 (1 to 32 bytes), 0 is default
	RFReg_RX_PW_P2     			= 0x13,		// RX payload width register: number of bytes in RX payload in data pipe 2 (1 to 32 bytes), 0 is default
	RFReg_RX_PW_P3     			= 0x14,		// RX payload width register: number of bytes in RX payload in data pipe 3 (1 to 32 bytes), 0 is default
	RFReg_RX_PW_P4     			= 0x15,		// RX payload width register: number of bytes in RX payload in data pipe 4 (1 to 32 bytes), 0 is default
	RFReg_RX_PW_P5     			= 0x16,		// RX payload width register: number of bytes in RX payload in data pipe 5 (1 to 32 bytes), 0 is default
	RFReg_FIFO_STATUS  			= 0x17,		// FIFO status register
	RFReg_DYNPD        			= 0x1C,		// Enable dynamic payload length register
	RFReg_FEATURE      			= 0x1D,		// Feature register
	#ifdef RF_SE8R01
	RFReg_SETUP_VALUE      		= 0x1E,		//
	RFReg_PRE_GURD      		= 0x1F,		//
	#endif

	#ifdef RF_RFM7X
	RFReg1_MustWrite00			= 0x00,		// 0w Must write with 0x404B01E2
	RFReg1_MustWrite01			= 0x01,		// 0w Must write with 0xC04B0000
	RFReg1_MustWrite02			= 0x02,		// 0w Must write with 0xD0FC8C02
	RFReg1_MustWrite03			= 0x03,		// 0x03001200w Must write with 0x99003921
	RFReg1_MustWrite04			= 0x04,		// 0w Must write with: 1Msps - 0xF996821B, 2Msps - 0xF99682DB, 250ksps - 0xF9968ADB, For single carrier mode - 0xF9968221
	RFReg1_MustWrite05			= 0x05,		// 0w Must write with: 1Msps - 0x24060FA6 (Disable RSSI), 2Msps - 0x 24060FB6 (Disable RSSI), 250ksps - 0x24060FB6 (Disable RSSI). RSSI bit is 18
	RFReg1_RBANK				= 0x07,		// 0r Register bank selection states. Switch register bank is done by SPI command "ACTIVATE" followed by 0x53: 0 - Register bank 0, 1 - Register bank 1
	RFReg1_CHIPID				= 0x08,		// 0x00000063r BEKEN Chip ID: 0x00000063 (RFM)
	RFReg1_MustWrite0C			= 0x0C,		// Please initialize with 0x05731200 For 120us mode:0x00731200
	RFReg1_NEW_FEATURE			= 0x0D,		// Please initialize with 0x0080B436
	RFReg1_RAMP					= 0x0E,		// Ramp curve Please write with 0xFFFFFEF7CF208104082041
	#endif
} eRF_Register;

// CONFIG: Configuration register
typedef enum {
	RFConfig_PRIM_RX			= (1 << 0),	// 0rw RX/TX control: 0 - PTX, 1 - PRX
	RFConfig_PWR_UP				= (1 << 1),	// 0rw Power control: 0 - POWER DOWN, 1 - POWER UP
	RFConfig_CRCO				= (1 << 2),	// 0rw CRC encoding scheme: 0 - 1 byte, 1 - 2 bytes
	RFConfig_EN_CRC				= (1 << 3),	// 1rw Enable CRC. Forced high if one of the bits in the EN_AA is high
	RFConfig_MASK_MAX_RT		= (1 << 4),	// 0rw Mask interrupt caused by MAX_RT: 1 - Interrupt not reflected on the IRQ pin, 0 - Reflect MAX_RT as active low interrupt on the IRQ pin
	RFConfig_MASK_TX_DS			= (1 << 5),	// 0rw Mask interrupt caused by TX_DS: 1 - Interrupt not reflected on the IRQ pin, 0 - Reflect TX_DS as active low interrupt on the IRQ pin
	RFConfig_MASK_RX_DR			= (1 << 6),	// 0rw Mask interrupt caused by RX_DR: 1 - Interrupt not reflected on the IRQ pin, 0 - Reflect RX_DR as active low interrupt on the IRQ pin
} eRF_Config;

// SETUP_AW: Setup of address widths (common for all data pipes) register
typedef enum {
	RFSetupAW_3Bytes			= (1 << 0),	// rw 3Bytes
	RFSetupAW_4Bytes			= (2 << 0),	// rw 4Bytes
	RFSetupAW_5Bytes			= (3 << 0),	// rw 5Bytes (Default)
} eRF_SetupAW;

// SETUP_RETR: Setup of automatic retransmission register
typedef enum {
	RFSetupRetr_ARC_Disable			= (0 << 0),
	RFSetupRetr_ARC_1Time			= (1 << 0),
	RFSetupRetr_ARC_2Time			= (2 << 0),
	RFSetupRetr_ARC_3Time			= (3 << 0),	// Default
	RFSetupRetr_ARC_4Time			= (4 << 0),
	RFSetupRetr_ARC_5Time			= (5 << 0),
	RFSetupRetr_ARC_6Time			= (6 << 0),
	RFSetupRetr_ARC_7Time			= (7 << 0),
	RFSetupRetr_ARC_8Time			= (8 << 0),
	RFSetupRetr_ARC_9Time			= (9 << 0),
	RFSetupRetr_ARC_10Time			= (10 << 0),
	RFSetupRetr_ARC_11Time			= (11 << 0),
	RFSetupRetr_ARC_12Time			= (12 << 0),
	RFSetupRetr_ARC_13Time			= (13 << 0),
	RFSetupRetr_ARC_14Time			= (14 << 0),
	RFSetupRetr_ARC_15Time			= (15 << 0),
	RFSetupRetr_ARC__Mask			= (15 << 0),
	RFSetupRetr_ARD_250us			= (0 << 4),	// Default
	RFSetupRetr_ARD_500us			= (1 << 4),
	RFSetupRetr_ARD_750us			= (2 << 4),
	RFSetupRetr_ARD_1000us			= (3 << 4),
	RFSetupRetr_ARD_1250us			= (4 << 4),
	RFSetupRetr_ARD_1500us			= (5 << 4),
	RFSetupRetr_ARD_1750us			= (6 << 4),
	RFSetupRetr_ARD_2000us			= (7 << 4),
	RFSetupRetr_ARD_2250us			= (8 << 4),
	RFSetupRetr_ARD_2500us			= (9 << 4),
	RFSetupRetr_ARD_2750us			= (10 << 4),
	RFSetupRetr_ARD_3000us			= (11 << 4),
	RFSetupRetr_ARD_3250us			= (12 << 4),
	RFSetupRetr_ARD_3500us			= (13 << 4),
	RFSetupRetr_ARD_3750us			= (14 << 4),
	RFSetupRetr_ARD_4000us			= (15 << 4),
	RFSetupRetr_ARD__Mask			= (15 << 4),
} eRF_SetupRetr;

// RF_SETUP: RF setup register
typedef enum {
	RFRFSetup_LNA_HCURR				= (1 << 0),	// 1rw Setup LNA gain: 0 - Low gain(20dB down), 1 - High gain
	RFRFSetup_RF_PWR_Low			= (0 << 1),	// rw Set RFM75 output power in TX mode as low
	RFRFSetup_RF_PWR_Econom			= (1 << 1),	// rw Set RFM75 output power in TX mode as econom
	RFRFSetup_RF_PWR_Good			= (2 << 1),	// rw Set RFM75 output power in TX mode as good
	RFRFSetup_RF_PWR_High			= (3 << 1),	// rw Set RFM75 output power in TX mode as high (Default)
	#if defined RF_RFM75
	RFRFSetup_RFM75_PWR_m10			= RFRFSetup_RF_PWR_Low,		// rw Set RFM75 output power in TX mode -10 dBm
	RFRFSetup_RFM75_PWR_m5			= RFRFSetup_RF_PWR_Econom,	// rw Set RFM75 output power in TX mode -5 dBm
	RFRFSetup_RFM75_PWR_0			= RFRFSetup_RF_PWR_Good,	// rw Set RFM75 output power in TX mode 0 dBm
	RFRFSetup_RFM75_PWR_5			= RFRFSetup_RF_PWR_High,	// rw Set RFM75 output power in TX mode 5 dBm (Default)
	#elif defined RF_NRF24
	RFRFSetup_NRF_PWR_m18			= RFRFSetup_RF_PWR_Low,		// rw Set nRF output power in TX mode -18 dBm
	RFRFSetup_NRF_PWR_m12			= RFRFSetup_RF_PWR_Econom,	// rw Set nRF output power in TX mode -12 dBm
	RFRFSetup_NRF_PWR_m6			= RFRFSetup_RF_PWR_Good,	// rw Set nRF output power in TX mode -6 dBm
	RFRFSetup_NRF_PWR_0				= RFRFSetup_RF_PWR_High,	// rw Set nRF output power in TX mode 0 dBm (Default)
	#elif defined RF_SE8R01
	RFRFSetup_NRF_PWR_m18			= RFRFSetup_RF_PWR_Low,		// rw Set nRF output power in TX mode -18 dBm
	RFRFSetup_NRF_PWR_m12			= RFRFSetup_RF_PWR_Econom,	// rw Set nRF output power in TX mode -12 dBm
	RFRFSetup_NRF_PWR_m6			= RFRFSetup_RF_PWR_Good,	// rw Set nRF output power in TX mode -6 dBm
	RFRFSetup_NRF_PWR_0				= RFRFSetup_RF_PWR_High,	// rw Set nRF output power in TX mode 0 dBm (Default)
	#endif
	RFRFSetup_RF_PWR__Mask			= (3 << 1),
	RFRFSetup_RF_DR_1Mbps			= (0 << 3),	// rw Set Air Data Rate as 1Mbps
	RFRFSetup_RF_DR_2Mbps			= (1 << 3),	// rw Set Air Data Rate as 2Mbps (Default)
	#ifdef RF_SE8R01
	RFRFSetup_RF_DR_500Kbps			= (4 << 3),	// rw Set Air Data Rate as 500Kbps
	#else
	RFRFSetup_RF_DR_250Kbps			= (4 << 3),	// rw Set Air Data Rate as 250Kbps
	#endif
	RFRFSetup_RF_DR__Mask			= (5 << 3),
	RFRFSetup_PLL_LOCK				= (1 << 4),	// 0rw Force PLL lock signal. Only used in test
	#ifdef RF_NRF24
	RFRFSetup_CONT_WAVE				= (1 << 7),	// 0rw Enables continuous carrier transmit when high
	#endif
} eRF_RFSetup;

// STATUS: Status register (In parallel to the SPI command word applied on the MOSI pin, the STATUS register is shifted serially out on the MISO pin)
typedef enum {
	RFStatus_TX_FULL				= (1 << 0),	// 0r TX FIFO full flag: 1 - TX FIFO full, 0 - Available locations in TX FIFO
	RFStatus_RX_P_NO__Shift			= 1,		// 7r Data pipe number for the payload available for reading from RX_FIFO: 000-101 - Data Pipe Number, 110 - Not used, 111 - RX FIFO Empty
	RFStatus_RX_P_NO__Mask			= (7 << RFStatus_RX_P_NO__Shift),
	RFStatus_MAX_RT					= (1 << 4),	// 0rw Maximum number of TX retransmits interrupt. Write 1 to clear bit. If MAX_RT is asserted it must be cleared to enable further communication
	RFStatus_TX_DS					= (1 << 5),	// 0rw Data Sent TX FIFO interrupt. Asserted when packet transmitted on TX. If AUTO_ACK is activated, this bit is set high only when ACK is received. Write 1 to clear bit
	RFStatus_RX_DR					= (1 << 6),	// 0rw Data Ready RX FIFO interrupt. Asserted when new data arrives RX FIFO. Write 1 to clear bit
	#if defined RF_RFM7X || defined RF_SE8R01
	RFStatus_RBANK					= (1 << 7),	// 0r Register bank selection states. Switch register bank is done by SPI command "ACTIVATE" followed by 0x53: 0 - Register bank 0, 1 - Register bank 1
	#endif
} eRF_Status;

// OBSERVE_TX: Transmit observe register
typedef enum {
	RFObserveTX_ARC_CNT__Shift		= 0,	// 0r Count retransmitted packets. The counter is reset when transmission of a new packet starts
	RFObserveTX_ARC_CNT__Mask		= (15 << RFObserveTX_ARC_CNT__Shift),
	RFObserveTX_PLOS_CNT__Shift		= 4,	// 0r Count lost packets. The counter is overflow protected to 15, and discontinues at max until reset. The counter is reset by writing to RF_CH
	RFObserveTX_PLOS_CNT__Mask		= (15 << RFObserveTX_PLOS_CNT__Shift),
} eRF_ObserveTX;

// CD: Carrier detect register
typedef enum {
	RF_CD_CD					= (1 << 0),	// 0r Carrier Detected
} eRF_CD;

// FIFO_STATUS: FIFO status register
typedef enum {
	RFFIFOStatus_RX_EMPTY		= (1 << 0),	// 1r RX FIFO empty flag: 1 - RX FIFO empty, 0 - Data in RX FIFO
	RFFIFOStatus_RX_FULL		= (1 << 1),	// 0r RX FIFO full flag: 1 - RX FIFO full, 0 - Available locations in RX FIFO
	RFFIFOStatus_TX_EMPTY		= (1 << 4),	// 1r TX FIFO empty flag: 1 - TX FIFO empty. 0 - Data in TX FIFO
	RFFIFOStatus_TX_FULL		= (1 << 5),	// 0r TX FIFO full flag: 1 - TX FIFO full, 0 - Available locations in TX FIFO
	RFFIFOStatus_TX_REUSE		= (1 << 6),	// 0r Reuse last transmitted data packet if set high. The packet is repeatedly retransmitted as long as CE is high. TX_REUSE is set by the SPI command REUSE_TX_PL, and is reset by the SPI command W_TX_PAYLOAD or FLUSH TX
} eRF_FIFOStatus;

// FEATURE: Feature register
typedef enum {
	RFFeature_EN_DYN_ACK		= (1 << 0),	// 0rw Enables the W_TX_PAYLOAD_NOACK command
	RFFeature_EN_ACK_PAY		= (1 << 1),	// 0rw Enables Payload with ACK
	RFFeature_EN_DPL			= (1 << 2),	// 0rw Enables Dynamic Payload Length
} eRF_Feature;

#ifdef RF_RFM7X
// MustWrite05:
typedef enum {
	RFMustWrite05_RSSI_EN		= (1 << 18),// 0w RSSI measurement: 0 - Enable, 1 - Disable
} eRF_MustWrite05;

// MustWrite0C:
typedef enum {
	RFMustWrite0C_CompatibleMode		= (1 << 9),		// Compatible mode: 0 - Static compatible, 1 - Dynamic compatible
	RFMustWrite0C_PLLSettings_120us		= (0 << 24),	// PLL Settling time 120us
	RFMustWrite0C_PLLSettings_130us		= (5 << 24),	// PLL Settling time 130us
} eRF_MustWrite0C;
#endif

// Pipes values used in ENAA and EN_RX_ADDR registers
typedef enum {
	RFPipe_0					= (1 << 0),	// Pipe 0
	RFPipe_1					= (1 << 1),	// Pipe 1
	RFPipe_2					= (1 << 2),	// Pipe 2
	RFPipe_3					= (1 << 3),	// Pipe 3
	RFPipe_4					= (1 << 4),	// Pipe 4
	RFPipe_5					= (1 << 5),	// Pipe 5
} eRF_Pipe;

typedef struct {
	eRF_Config CONFIG;
	eRF_Pipe ENAA;
	eRF_Pipe EN_RX_ADDR;
	eRF_SetupAW SETUP_AW;
	eRF_SetupRetr SETUP_RETR;
	uint8_t RF_CH;
	eRF_RFSetup RF_SETUP;
	eRF_Feature FEATURE;
	uint8_t RX_ADDR_P0[5];
	uint8_t RX_ADDR_P1[5];
	uint8_t RX_ADDR_P2;
	uint8_t RX_ADDR_P3;
	uint8_t RX_ADDR_P4;
	uint8_t RX_ADDR_P5;
	uint8_t RX_PW_P0;
	uint8_t RX_PW_P1;
	uint8_t RX_PW_P2;
	uint8_t RX_PW_P3;
	uint8_t RX_PW_P4;
	uint8_t RX_PW_P5;
} sRF_Settings;

typedef struct {
	sRF_Settings Settings;
	eRF_Status STATUS;
	eRF_FIFOStatus FIFO_STATUS;
	eRF_Pipe DYNPD;
} sRF_ConfigInfo;

typedef union {
	struct {
		uint8_t ARC:4;
		uint8_t PLOS:4;
	};
	uint8_t value;
} sRF_LostPacketStat;

typedef struct {
	void (*CE)(bool);
	void (*CS)(bool);
	bool (*IRQ)();
	uint8_t (*ReadWrite)(uint8_t);
} sRF_SPI;

// Detects the presence of RFM module
bool RF_IsPresent(sRF_SPI const *rfspi);
// Setup CONFIG parameters
void RF_Config(sRF_SPI const *rfspi, eRF_Config config);
// Enables auto-acknowledge feature of specified receive pipelines
void RF_PipelinesAutoACK(sRF_SPI const *rfspi, eRF_Pipe pipes);
// Enables specified receive pipelines therefore enabling communication with addresses connected to these pipelines
void RF_PipelinesEnable(sRF_SPI const *rfspi, eRF_Pipe pipes);
// Setup of address widths (common for all data pipes) register
void RF_SetupAW(sRF_SPI const *rfspi, eRF_SetupAW setup_aw);
// Setup of automatic retransmission register
void RF_SetupRetr(sRF_SPI const *rfspi, eRF_SetupRetr setup_retr);
// Setup current RF channel within 2400-2483.5MHz frequency window
void RF_SetChannel(sRF_SPI const *rfspi, uint8_t channel);
// Setup RF paramters
void RF_SetupRF(sRF_SPI const *rfspi, eRF_RFSetup rf_setup);
// Setup special feature paramters
void RF_Feature(sRF_SPI const *rfspi, eRF_Feature feature);
// Apply config
void RF_ApplySettings(sRF_SPI const *rfspi, sRF_Settings const *settings);
// Get configuration info
void RF_GetConfigInfo(sRF_SPI const *rfspi, sRF_ConfigInfo *config_info);

// Change RFM mode
void RF_Mode_PowerDown(sRF_SPI const *rfspi, bool is_flush);
void RF_Mode_Standby(sRF_SPI const *rfspi);
void RF_Mode_Transmit(sRF_SPI const *rfspi, bool is_flush);
void RF_Mode_Receive(sRF_SPI const *rfspi, bool is_flush, uint8_t const *address_p0);
// Get lost packets statistic
sRF_LostPacketStat RF_GetLostPacketStat(sRF_SPI const *rfspi);

// Checks whether transmitter buffer is full
bool RF_IsTransmitterBufferFull(sRF_SPI const *rfspi);
// Checks whether receiver buffer is empty
bool RF_IsReceiverBufferEmpty(sRF_SPI const *rfspi);
#ifdef RF_SE8R01
// Get carrier signal level
uint8_t RF_CarrierLevel(sRF_SPI const *rfspi);
#else
// Checks whether carrier detected
bool RF_IsCarrierDetected(sRF_SPI const *rfspi);
#endif
// Set TX address
void RF_TransmitterAddress(sRF_SPI const *rfspi, uint8_t const *address, bool is_ack);
// Transmits a packet
bool RF_PackageTransmit(sRF_SPI const *rfspi, bool is_ack, void const *src, uint8_t len);
// Receives a packet
uint8_t RF_PackageReceive(sRF_SPI const *rfspi, void *dst, uint8_t *len, uint32_t *timeout_10us);

#ifdef __cplusplus
}
#endif
