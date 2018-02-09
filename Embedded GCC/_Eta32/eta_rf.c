/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_rf.c
 ***********************************************************************/

#include "eta_rf.h"
#include "eta_delay.h"
#include "eta_dbg.h"

#ifdef RF_RFM7X
// Bank1 register initialization value. Some magic numbers here duplicates data from datasheet, some of the byte reversed. DO NOT edit this array
static uint8_t const RF_Bank1_Reg0_13_Regs[] = {
	0x00,
	0x01,
	0x02,
	0x03,
	0x04,
	0x05,
	0x0C,
	0x0D,
};
static uint32_t const RF_Bank1_Reg0_13_Values[] = {
	#if defined RF_RFM75
	0xE2014B40,		// 0	|
	0x00004BC0,		// 1	|
	0x028CFCD0,		// 2	|		Already reversed!
	0x21390099,		// 3	|
	0x1B8296F9,		// 4	|	250k - DB8A96F9, 1M - 1B8296F9, 2M - DB8296F9
	0xA60F0624,		// 5	|	250k - B60F0624, 1M - A60F0624, 2M - B60F0624
	0x05731200,		// 12
	0x0080B436,		// 13
	#endif
};
// Bank1 register 14 initialization value. More magic numbers, once again: DO NOT EDIT this array
static uint8_t const RF_Bank1_RAMP[] = {
	#if defined RF_RFM75
	0x41, 0x20, 0x08, 0x04, 0x81, 0x20, 0xCF, 0xF7, 0xFE ,0xFF, 0xFF
	#endif
};
#endif

#ifdef RF_SE8R01
// SETUP_VALUE initialization value
static uint8_t const RF_Bank0_SETUP_VALUE[] = {
	0x28, 0x32, 0x80, 0x90, 0x00
};
// PRE_GURD initialization value
static uint8_t const RF_Bank0_PRE_GURD[] = {
	0x00, 0x00
};
#endif

typedef struct {
	uint8_t value;
	eRF_Status status;
} sRFMCmdRes;

static eRF_Status RF_WriteCmd(sRF_SPI const *rfspi, eRF_Command cmd) {
	rfspi->CS(true);
	eRF_Status _status = rfspi->ReadWrite(cmd);
	rfspi->CS(false);
	return _status;
}
static eRF_Status RF_WriteCmd1(sRF_SPI const *rfspi, eRF_Command cmd, uint8_t value) {
	rfspi->CS(true);
	eRF_Status _status = rfspi->ReadWrite(cmd); rfspi->ReadWrite(value);
	rfspi->CS(false);
	return _status;
}
static sRFMCmdRes RF_ReadCmd1(sRF_SPI const *rfspi, eRF_Command cmd) {
	sRFMCmdRes _res;
	rfspi->CS(true);
	_res.status = rfspi->ReadWrite(cmd); _res.value = rfspi->ReadWrite(0);
	rfspi->CS(false);
	return _res;
}
static eRF_Status RF_WriteCmdBlock(sRF_SPI const *rfspi, eRF_Command cmd, void const *src, uint8_t len) {
	rfspi->CS(true);
	eRF_Status _status = rfspi->ReadWrite(cmd);
	uint8_t const *_src = (uint8_t const *)src;
	while(len--) rfspi->ReadWrite(*_src++);
	rfspi->CS(false);
	return _status;
}
static eRF_Status RF_ReadCmdBlock(sRF_SPI const *rfspi, eRF_Command cmd, void *dst, uint8_t len) {
	rfspi->CS(true);
	eRF_Status _status = rfspi->ReadWrite(cmd);
	uint8_t *_dst = (uint8_t *)dst;
	while(len--) *_dst++ = rfspi->ReadWrite(0);
	rfspi->CS(false);
	return _status;
}
static eRF_Status RF_StatusRead(sRF_SPI const *rfspi) {
	rfspi->CS(true);
	eRF_Status _status_ret = rfspi->ReadWrite(RFCommand_R_REGISTER | RFReg_STATUS);
	rfspi->CS(false);
	return _status_ret;
}
static eRF_Status RF_StatusReadClear(sRF_SPI const *rfspi, eRF_Status status_clear) {
	rfspi->CS(true);
	eRF_Status _status_ret = rfspi->ReadWrite(RFCommand_W_REGISTER | RFReg_STATUS);
	rfspi->ReadWrite(status_clear);
	rfspi->CS(false);
	return _status_ret;
}
static eRF_Status RF_StatusClear(sRF_SPI const *rfspi) {
	rfspi->CS(true);
	eRF_Status _status = rfspi->ReadWrite(RFCommand_W_REGISTER | RFReg_STATUS);
	rfspi->ReadWrite(_status);
	rfspi->CS(false);
	return _status;
}
#ifdef RF_RFM7X
static void RF_SelectRBank(sRF_SPI const *rfspi, bool is_bank1) {
	bool _is_bank1_now = RF_StatusRead(rfspi) & RFStatus_RBANK;
	// toggle reg bank
	if(is_bank1 != _is_bank1_now) { RF_WriteCmd1(rfspi, RFCommand_ACTIVATE, 0x53); _delay_us(10); }
}
#endif

// Detects the presence of RFM module
bool RF_IsPresent(sRF_SPI const *rfspi) {
	#if defined RF_RFM7X || defined RF_SE8R01
	eRF_Status _st1, _st2;
	_st1 = RF_WriteCmd1(rfspi, RFCommand_ACTIVATE, 0x53);
	_delay_us(10);
	_st2 = RF_WriteCmd1(rfspi, RFCommand_ACTIVATE, 0x53);
	return (_st1 ^ _st2) == RFStatus_RBANK;
	#else
	return true;
	#endif
}
// Setup CONFIG parameters
void RF_Config(sRF_SPI const *rfspi, eRF_Config config) { RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_CONFIG, config); }
// Enables auto-acknowledge feature of specified receive pipelines
void RF_PipelinesAutoACK(sRF_SPI const *rfspi, eRF_Pipe pipes) { RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_ENAA, pipes); }
// Enables specified receive pipelines therefore enabling communication with addresses connected to these pipelines
void RF_PipelinesEnable(sRF_SPI const *rfspi, eRF_Pipe pipes) { RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_EN_RX_ADDR, pipes); }
// Setup of address widths (common for all data pipes) register
void RF_SetupAW(sRF_SPI const *rfspi, eRF_SetupAW setup_aw) { RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_SETUP_AW, setup_aw); }
// Setup of automatic retransmission register
void RF_SetupRetr(sRF_SPI const *rfspi, eRF_SetupRetr setup_retr) { RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_SETUP_RETR, setup_retr); }
// Setup current RF channel within 2400-2483.5MHz frequency window
void RF_SetChannel(sRF_SPI const *rfspi, uint8_t channel) { RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_RF_CH, channel); }
// Setup RF paramters
void RF_SetupRF(sRF_SPI const *rfspi, eRF_RFSetup rf_setup) { RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_RF_SETUP, rf_setup); }
// Setup special feature paramters
void RF_Feature(sRF_SPI const *rfspi, eRF_Feature feature) { RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_FEATURE, feature); }
// Apply config
void RF_ApplySettings(sRF_SPI const *rfspi, sRF_Settings const *settings) {
	DBG_PutString("Applying RF settings" NL);
	rfspi->CE(false);
	RF_StatusClear(rfspi);

	#ifdef RF_RFM7X
	if (!RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_FEATURE).value) {
		// module is not activated. activation sequence
		RF_WriteCmd1(rfspi, RFCommand_ACTIVATE, 0x73);
	}
	RF_SelectRBank(rfspi, true);
	uint8_t const *_regs1_reg = RF_Bank1_Reg0_13_Regs; uint32_t const *_regs1_value = RF_Bank1_Reg0_13_Values; uint32_t _cnt = sizeof_array(RF_Bank1_Reg0_13_Regs);
	while(_cnt--) { RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | *_regs1_reg++, _regs1_value++, 4); }
	RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | RFReg1_RAMP, RF_Bank1_RAMP, sizeof_array(RF_Bank1_RAMP));
	static uint32_t const _tog1 = 0xDB8296FF, _tog2 = 0xDB8296F9;
	RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | 4, &_tog1, sizeof(_tog1));
	RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | 4, &_tog2, sizeof(_tog2));
	RF_SelectRBank(rfspi, false);
	#endif

	#ifdef RF_SE8R01
	RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | RFReg_SETUP_VALUE, RF_Bank0_SETUP_VALUE, sizeof_array(RF_Bank0_SETUP_VALUE));
	RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | RFReg_PRE_GURD, RF_Bank0_PRE_GURD, sizeof_array(RF_Bank0_PRE_GURD));
	#endif

	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_CONFIG, settings->CONFIG);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_ENAA, settings->ENAA);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_EN_RX_ADDR, settings->EN_RX_ADDR);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_SETUP_AW, settings->SETUP_AW);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_SETUP_RETR, settings->SETUP_RETR);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_RF_CH, settings->RF_CH);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_RF_SETUP, settings->RF_SETUP);
	RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | RFReg_RX_ADDR_P0, settings->RX_ADDR_P0, 5);
	RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | RFReg_RX_ADDR_P1, settings->RX_ADDR_P1, 5);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_RX_ADDR_P2, settings->RX_ADDR_P2);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_RX_ADDR_P3, settings->RX_ADDR_P3);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_RX_ADDR_P4, settings->RX_ADDR_P4);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_RX_ADDR_P5, settings->RX_ADDR_P5);
	eRF_Pipe _dynpd = 0;
	__extension__ void _write_pw(uint8_t reg, uint8_t value, eRF_Pipe pipe) { if(value == (uint8_t)-1) { value = 0x20; _dynpd |= pipe; } RF_WriteCmd1(rfspi, reg, value); };
	_write_pw(RFCommand_W_REGISTER | RFReg_RX_PW_P0, settings->RX_PW_P0, RFPipe_0);
	_write_pw(RFCommand_W_REGISTER | RFReg_RX_PW_P1, settings->RX_PW_P1, RFPipe_1);
	_write_pw(RFCommand_W_REGISTER | RFReg_RX_PW_P2, settings->RX_PW_P2, RFPipe_2);
	_write_pw(RFCommand_W_REGISTER | RFReg_RX_PW_P3, settings->RX_PW_P3, RFPipe_3);
	_write_pw(RFCommand_W_REGISTER | RFReg_RX_PW_P4, settings->RX_PW_P4, RFPipe_4);
	_write_pw(RFCommand_W_REGISTER | RFReg_RX_PW_P5, settings->RX_PW_P5, RFPipe_5);
	eRF_Feature _feature = settings->FEATURE;
	if(_dynpd) _feature |= RFFeature_EN_DPL;
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_FEATURE, settings->FEATURE);
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_DYNPD, _dynpd);
	RF_StatusClear(rfspi);
}
// Get configuration info
void RF_GetConfigInfo(sRF_SPI const *rfspi, sRF_ConfigInfo *config_info) {
	config_info->STATUS = RF_StatusRead(rfspi);
	config_info->Settings.CONFIG = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_CONFIG).value;
	config_info->Settings.ENAA = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_ENAA).value;
	config_info->Settings.EN_RX_ADDR = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_EN_RX_ADDR).value;
	config_info->Settings.SETUP_AW = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_SETUP_AW).value;
	config_info->Settings.SETUP_RETR = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_SETUP_RETR).value;
	config_info->Settings.RF_CH = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RF_CH).value;
	config_info->Settings.RF_SETUP = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RF_SETUP).value;
	RF_ReadCmdBlock(rfspi, RFCommand_R_REGISTER | RFReg_RX_ADDR_P0, config_info->Settings.RX_ADDR_P0, 5);
	RF_ReadCmdBlock(rfspi, RFCommand_R_REGISTER | RFReg_RX_ADDR_P1, config_info->Settings.RX_ADDR_P1, 5);
	config_info->Settings.RX_ADDR_P2 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_ADDR_P2).value;
	config_info->Settings.RX_ADDR_P3 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_ADDR_P3).value;
	config_info->Settings.RX_ADDR_P4 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_ADDR_P4).value;
	config_info->Settings.RX_ADDR_P5 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_ADDR_P5).value;
	config_info->Settings.RX_PW_P0 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_PW_P0).value;
	config_info->Settings.RX_PW_P1 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_PW_P1).value;
	config_info->Settings.RX_PW_P2 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_PW_P2).value;
	config_info->Settings.RX_PW_P3 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_PW_P3).value;
	config_info->Settings.RX_PW_P4 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_PW_P4).value;
	config_info->Settings.RX_PW_P5 = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RX_PW_P5).value;
	config_info->Settings.FEATURE = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_FEATURE).value;
	config_info->FIFO_STATUS = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_FIFO_STATUS).value;
	config_info->DYNPD = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_DYNPD).value;
}

// Change RFM mode
void RF_Mode_PowerDown(sRF_SPI const *rfspi, bool is_flush) {
	// flush transmit and receive queue
	if(is_flush) { RF_WriteCmd(rfspi, RFCommand_FLUSH_TX); RF_WriteCmd(rfspi, RFCommand_FLUSH_RX); }
	rfspi->CE(false);
	// reset PWR_UP bit in CONFIG register
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_CONFIG, RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_CONFIG).value & ~RFConfig_PWR_UP);
}
void RF_Mode_Standby(sRF_SPI const *rfspi) {
	rfspi->CE(false);
	// set PWR_UP bit in CONFIG register
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_CONFIG, RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_CONFIG).value | RFConfig_PWR_UP);
	_delay_ms(10);
}
void RF_Mode_Transmit(sRF_SPI const *rfspi, bool is_flush) {
	// flush transmit queue
	if(is_flush) RF_WriteCmd(rfspi, RFCommand_FLUSH_TX);
	// clear interrupt status
	RF_StatusClear(rfspi);
	rfspi->CE(false);
	// set PWR_UP bit and clear RX bit in CONFIG register
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_CONFIG, (RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_CONFIG).value | RFConfig_PWR_UP) & ~RFConfig_PRIM_RX);
	rfspi->CE(true);
	_delay_us(150);
}
void RF_Mode_Receive(sRF_SPI const *rfspi, bool is_flush, uint8_t const *address_p0) {
	// flush receive queue
	if(is_flush) RF_WriteCmd(rfspi, RFCommand_FLUSH_RX);
	// clear interrupt status
	RF_StatusClear(rfspi);
	rfspi->CE(false);
	// set pipe 0 receive address if needed
	if(address_p0) RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | RFReg_RX_ADDR_P0, address_p0, 5);
	// set PWR_UP and RX bit in CONFIG register
	RF_WriteCmd1(rfspi, RFCommand_W_REGISTER | RFReg_CONFIG, RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_CONFIG).value | RFConfig_PWR_UP | RFConfig_PRIM_RX);
	rfspi->CE(true);
	_delay_us(150);
}

// Get lost packets statistic
sRF_LostPacketStat RF_GetLostPacketStat(sRF_SPI const *rfspi) {
	sRF_LostPacketStat _stat;
	_stat.value = RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_OBSERVE_TX).value;
	return _stat;
}

// Checks whether transmitter buffer is full
bool RF_IsTransmitterBufferFull(sRF_SPI const *rfspi) { return RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_FIFO_STATUS).value & RFFIFOStatus_TX_FULL; }
// Checks whether receiver buffer is empty
bool RF_IsReceiverBufferEmpty(sRF_SPI const *rfspi) { return RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_FIFO_STATUS).value & RFFIFOStatus_RX_EMPTY; }
#ifdef RF_SE8R01
// Get carrier signal level
uint8_t RF_CarrierLevel(sRF_SPI const *rfspi) { return RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_RPD).value; }
#else
// Checks whether carrier detected
bool RF_IsCarrierDetected(sRF_SPI const *rfspi) { return RF_ReadCmd1(rfspi, RFCommand_R_REGISTER | RFReg_CD).value & RF_CD_CD; }
#endif
// Set TX address
void RF_TransmitterAddress(sRF_SPI const *rfspi, uint8_t const *address, bool is_ack) {
	RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | RFReg_TX_ADDR, address, 5);
	if(is_ack) RF_WriteCmdBlock(rfspi, RFCommand_W_REGISTER | RFReg_RX_ADDR_P0, address, 5);
}
// Transmits a packet
bool RF_PackageTransmit(sRF_SPI const *rfspi, bool is_ack, void const *src, uint8_t len) {
	// loop while transmitter buffer is full
	while(RF_IsTransmitterBufferFull(rfspi));
	// packet with acknowlage
	if(is_ack) {
		RF_WriteCmdBlock(rfspi, RFCommand_W_TX_PAYLOAD, src, len);
		// wait for acknowlage or MAX_RT error
		while(!rfspi->IRQ());
	}
	// no acknowlage packet
	else {
		RF_WriteCmdBlock(rfspi, RFCommand_W_TX_PAYLOAD_NO_ACK, src, len);
		uint32_t _retries = 6000;
		// wait for transmition to complete
		while(!rfspi->IRQ() && _retries--) _delay_us(10);
	}
	eRF_Status _status = RF_StatusReadClear(rfspi, RFStatus_MAX_RT | RFStatus_TX_DS);
	if(_status & RFStatus_TX_DS) return true;
	if(_status & RFStatus_MAX_RT) { RF_WriteCmd(rfspi, RFCommand_FLUSH_TX); }
	return false;
}
// Receives a packet
uint8_t RF_PackageReceive(sRF_SPI const *rfspi, void *dst, uint8_t *len, uint32_t *timeout_10us) {
	while(true) {
		if(rfspi->IRQ()) {
			eRF_Status _status = RF_StatusRead(rfspi);
			if(_status & RFStatus_RX_DR) {
				sRFMCmdRes _res = RF_ReadCmd1(rfspi, RFCommand_R_RX_PL_WID);
				uint8_t _pipe = (_res.status & RFStatus_RX_P_NO__Mask) >> RFStatus_RX_P_NO__Shift;
				uint8_t _len = *len = _res.value;
				RF_ReadCmdBlock(rfspi, RFCommand_R_RX_PAYLOAD, dst, _len);
				if(RF_IsReceiverBufferEmpty(rfspi)) RF_StatusReadClear(rfspi, RFStatus_RX_DR);
				return _pipe;
			}
			return (uint8_t)-1;
		}
		if(timeout_10us) {
			uint32_t _timeout_10us = *timeout_10us;
			if(_timeout_10us) { _timeout_10us--; *timeout_10us = _timeout_10us; _delay_us(10); }
			else return (uint8_t)-1;
		}
	}
}
