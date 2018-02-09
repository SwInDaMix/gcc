/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : entry.cpp
 ***********************************************************************/

#include "config.h"
#include "stdperiph.h"
#include "entry.h"
#include "eta_crc.h"
#include "eta_dbg.h"
#include "eta_string_format.h"
#include "eta_frames.h"
#include "eta_delay.h"

#ifdef SECURITY_SERVER
void RFMSPIServer_CE(bool state) { if(state) { SPI_RF_SERVER_CE_PORT->BSRR = (1 << SPI_RF_SERVER_CE_PIN); } else { SPI_RF_SERVER_CE_PORT->BRR = (1 << SPI_RF_SERVER_CE_PIN); } }
void RFMSPIServer_CS(bool state) { if(state) { SPI_RF_SERVER_NSS_PORT->BRR = (1 << SPI_RF_SERVER_NSS_PIN); } else { SPI_RF_SERVER_NSS_PORT->BSRR = (1 << SPI_RF_SERVER_NSS_PIN); } }
bool RFMSPIServer_IRQ() { return !(SPI_RF_SERVER_IRQ_PORT->IDR & (1 << SPI_RF_SERVER_IRQ_PIN)); }
uint8_t RFMSPIServer_ReadWrite(uint8_t value) {
	__IO uint8_t * _dr = (__IO uint8_t *)&SPI_RF_SERVER->DR;
	*_dr = value;
	while (!(SPI_RF_SERVER->SR & SPI_SR_RXNE));
	return (*_dr);
}
sRF_SPI const RFMSPIServer = { .CE = RFMSPIServer_CE, .CS = RFMSPIServer_CS, .IRQ = RFMSPIServer_IRQ, .ReadWrite = RFMSPIServer_ReadWrite };
#endif

#ifdef SECURITY_CLIENT
void RFMSPIClient_CE(bool state) { if(state) { SPI_RF_CLIENT_CE_PORT->BSRR = (1 << SPI_RF_CLIENT_CE_PIN); } else { SPI_RF_CLIENT_CE_PORT->BRR = (1 << SPI_RF_CLIENT_CE_PIN); } }
void RFMSPIClient_CS(bool state) { if(state) { SPI_RF_CLIENT_NSS_PORT->BRR = (1 << SPI_RF_CLIENT_NSS_PIN); } else { SPI_RF_CLIENT_NSS_PORT->BSRR = (1 << SPI_RF_CLIENT_NSS_PIN); } }
bool RFMSPIClient_IRQ() { return !(SPI_RF_CLIENT_IRQ_PORT->IDR & (1 << SPI_RF_CLIENT_IRQ_PIN)); }
uint8_t RFMSPIClient_ReadWrite(uint8_t value) {
	__IO uint8_t * _dr = (__IO uint8_t *)&SPI_RF_CLIENT->DR;
	*_dr = value;
	while (!(SPI_RF_CLIENT->SR & SPI_SR_RXNE));
	return (*_dr);
}
sRF_SPI const RFMSPIClient = { .CE = RFMSPIClient_CE, .CS = RFMSPIClient_CS, .IRQ = RFMSPIClient_IRQ, .ReadWrite = RFMSPIClient_ReadWrite };
#endif

void LogEvent(eLog log, char const *fmt, ...) {
	//////////////////////////////////////////////////////////////////////////
	__extension__ void log_putc(sFmtStream *strm, char c) { DBG_PutChar(c); };
	//////////////////////////////////////////////////////////////////////////

	DBG_PutString(log == Log_OrdinalEvent ? "OrdinalEvent" : log == Log_SecurityAlert ? "SecurityAlert" : log == Log_SystemError ? "SystemError" : log == Log_HardwareFailure ? "HardwareFailure" : "<unknown>");
	DBG_PutString(" - ");
	va_list _ap; va_start(_ap, fmt); sFmtStream _strm = { .putc = log_putc, .limit = -1, .flags = (sFormatFlags)0 }; formatString(fmt, _ap, &_strm); va_end(_ap);
	DBG_PutString(NL);
}

static uint32_t volatile TimerMs;
#ifdef SECURITY_SERVER
static uint32_t volatile EncoderCounter;
#endif

#ifdef SECURITY_CLIENT
static volatile eSecurityActionButtons SecurityActionButtonsState = 0;
static volatile eSecurityActionPressButtons SecurityActionButtons = eSecurityActionPressButtons_None;
static volatile bool SecurityActionButtonsFault = false;
static volatile uint32_t SecurityClientLEDRed = 0, SecurityClientLEDGreen = 0;
#endif

void PeriodicHandler() {
	TimerMs++;

	#ifdef SECURITY_SERVER
	static uint8_t _encoder_t1_cnt = 0, _encoder_t2_cnt = 0;
	if((ENCODER_T1_PORT->IDR & (1 << ENCODER_T1_PIN))) { _encoder_t1_cnt = 0; } else { if(_encoder_t1_cnt < 5) { _encoder_t1_cnt++; if(_encoder_t1_cnt == 5 && _encoder_t2_cnt == 5) EncoderCounter++; } }
	if((ENCODER_T2_PORT->IDR & (1 << ENCODER_T2_PIN))) { _encoder_t2_cnt = 0; } else { if(_encoder_t2_cnt < 5) { _encoder_t2_cnt++; if(_encoder_t2_cnt == 5 && _encoder_t1_cnt == 5) EncoderCounter--; } }
	#endif

	#ifdef SECURITY_CLIENT
	// Process buttons
	static eSecurityActionButtons _act_prev = 0; static eSecurityActionPressButtons _act_prev_simple = eSecurityActionPressButtons_None; static uint32_t _repeat = 0; static bool _is_button_event = false;
	eSecurityActionButtons _act_new = 0; eSecurityActionPressButtons _act_new_simple = eSecurityActionPressButtons_None;
	if(!(BUTTON_A_PORT->IDR & (1 << BUTTON_A_PIN))) { _act_new |= SecurityActionButton_0; _act_new_simple = eSecurityActionPressButtons_1; }
	if(!(BUTTON_B_PORT->IDR & (1 << BUTTON_B_PIN))) { _act_new |= SecurityActionButton_1; _act_new_simple = eSecurityActionPressButtons_2; }
	if(!(BUTTON_C_PORT->IDR & (1 << BUTTON_C_PIN))) { _act_new |= SecurityActionButton_2; _act_new_simple = eSecurityActionPressButtons_3; }
	if(!(BUTTON_D_PORT->IDR & (1 << BUTTON_D_PIN))) { _act_new |= SecurityActionButton_3; _act_new_simple = eSecurityActionPressButtons_4; }
	SecurityActionButtonsState = _act_new;
	if(SecurityActionButtons == eSecurityActionPressButtons_None && !SecurityActionButtonsFault && !_is_button_event) {
		if(!_act_new && _act_prev_simple != eSecurityActionPressButtons_None) {
			if(_repeat >= PERIOD_BUTTON_MIN_PRESS) { _is_button_event = true; SecurityActionButtons = _act_prev_simple; } _repeat = 0;
		}
		else if(!SecurityActionButtonsFault && _act_new) {
			if(_act_prev == _act_new) {
				if(_repeat++ >= PERIOD_BUTTON_LONG_PRESS) { _is_button_event = true; SecurityActionButtons = eSecurityActionPressButtons_Long1 + _act_prev - SecurityActionButton_0; _repeat = 0; }
			}
			else {
				eSecurityActionButtons _act_or = _act_prev | _act_new;
				if((_act_or ^ _act_new)) SecurityActionButtonsFault = true;
				else _repeat = 0;
			}
		}
	}
	_act_prev = _act_new; _act_prev_simple = _act_new_simple;
	if(!_act_new) { SecurityActionButtonsFault = _is_button_event = false; _act_prev_simple = eSecurityActionPressButtons_None; }

	// Process LEDs
	static uint32_t _led_ovf;
	if(_led_ovf++ >= PERIOD_1BIT_LED) {
		_led_ovf = 0;
		register uint32_t _led_rotated_red = SecurityClientLEDRed, _led_rotated_green = SecurityClientLEDGreen;
		if(_led_rotated_red & 1) { LED_RED_PORT->BSRR = (1 << LED_RED_PIN); } else { LED_RED_PORT->BRR = (1 << LED_RED_PIN); }
		__asm ("ror %0, %2" : "=r" (_led_rotated_red) : "0" (_led_rotated_red), "r" (1));
		if(_led_rotated_green & 1) { LED_GREEN_PORT->BSRR = (1 << LED_GREEN_PIN); } else { LED_GREEN_PORT->BRR = (1 << LED_GREEN_PIN); }
		__asm ("ror %0, %2" : "=r" (_led_rotated_green) : "0" (_led_rotated_green), "r" (1));
		SecurityClientLEDRed = _led_rotated_red; SecurityClientLEDGreen = _led_rotated_green;
	}
	#endif
}

void Flash_PageErase(void const *addr) {
	DBG_PutFormat("Erasing flash page at %08uX" NL, addr);
	// Watchdog reset
	IWDG_ReloadCounter();
	uint32_t _failures = 0;
	while(true) {
		FLASH->KEYR = FLASH_FKEY1; FLASH->KEYR = FLASH_FKEY2;
		FLASH->CR |= FLASH_CR_PER; FLASH->AR = (uint32_t)addr; FLASH->CR |= FLASH_CR_STRT;
		while ((FLASH->SR & FLASH_SR_BSY));
		FLASH->CR &= ~FLASH_CR_PER;
		FLASH->CR |= FLASH_CR_LOCK;
		uint32_t const *_addr = (uint32_t const*)addr; size_t _len = SECURITY_PAGE_SIZE / sizeof(uint32_t); uint32_t _val = 0xFFFFFFFF;
		while(_len--) { _val &= *_addr++; }
		if(_val == 0xFFFFFFFF) break;
		_failures++;
	}
	if(_failures) { LogEvent(Log_HardwareFailure, "Flash page erasing at %08uX at %ud try", addr); }
}
bool Flash_Write(void const *addr, void const *data, size_t len) {
	DBG_PutFormat("Writing to flash at %08uX %ud bytes" NL, addr, len);
	// Watchdog reset
	IWDG_ReloadCounter();
	FLASH->KEYR = FLASH_FKEY1; FLASH->KEYR = FLASH_FKEY2;
	len >>= 1; uint16_t volatile *_dst = (uint16_t volatile *)addr, *_src = (uint16_t *)data; bool _res = true;
	FLASH->CR |= FLASH_CR_PG;
	while(len--) {
		*_dst = *_src;
		while ((FLASH->SR & FLASH_SR_BSY));
		if(*_dst != *_src) { _res = false; LogEvent(Log_HardwareFailure, "Flash writing at %08uX : %04X <<< %04X", addr, *_dst, *_src); }
		_dst++; _src++;
	}
	FLASH->CR &= ~FLASH_CR_PG;
	FLASH->CR |= FLASH_CR_LOCK;
	return _res;
}

uint32_t Security_RandomGet() {
	uint32_t _rn = 0, _cnt = 64;
	CRC_CalcCRC(TIM_PERIODIC->CNT);
	CRC_CalcCRC(TimerMs);
	ADC_ChannelConfig(ADC1, ADC_Channel_TempSensor, ADC_SampleTime_1_5Cycles);
	while(_cnt--) {
		// Start ADC1 Software Conversion
		ADC_StartOfConversion(ADC1);
		// Wait for conversion complete
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		uint16_t _adc_val = ADC_GetConversionValue(ADC1);
		_rn = CRC_CalcCRC(_adc_val);
		// Clear EOC flag
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	}
	return _rn;
}

#ifdef SECURITY_CLIENT
static void ClientPowerLock(bool power_lock) { if(power_lock) { POWERLOCK_PORT->BSRR = (1 << POWERLOCK_PIN); } else { POWERLOCK_PORT->BRR = (1 << POWERLOCK_PIN); } }
static void ClientLEDs_Set(uint32_t led_red, uint32_t led_green) { SecurityClientLEDRed = led_red; SecurityClientLEDGreen = led_green; }
static uint32_t ClientBaterryVoltage() {
	#define ADC_RES_LOW 10
	#define ADC_RES_HIGH 33
	#define ADC_BITWIDTH 12
	#define ADC_TO_VOLTAGE(adc) ((V_CPU * (adc) * (ADC_RES_LOW + ADC_RES_HIGH)) / (ADC_RES_LOW * (1 << ADC_BITWIDTH)))

	ADC_ChannelConfig(ADC1, ADC_VOLTAGE_CHANNEL, ADC_SampleTime_239_5Cycles);
	uint32_t _cnt = 8; uint32_t _adc_val = 0;
	while(_cnt--) {
		ADC_StartOfConversion(ADC1);
		while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
		_adc_val += ADC_GetConversionValue(ADC1);
	}
	return ADC_TO_VOLTAGE(_adc_val >> 3);
}

void SecurityOrdinalEvent_ButtonsPressed(eSecurityActionButtons buttons) { ClientLEDs_Set(0x80000000, 0x00008000); LogEvent(Log_OrdinalEvent, "Buttons pressed: %X" NL, buttons); }
void SecurityOrdinalEvent_GeneratedNewID(sSecurityID const *id) { LogEvent(Log_OrdinalEvent, "Generating new accessor ID: %Q", id, sizeof(sSecurityID)); }
void SecurityOrdinalEvent_FullResetInitiated(sSecurityID const *id) { ClientLEDs_Set(0x55005500, 0x00550055); LogEvent(Log_OrdinalEvent, "Accessor %Q initiates full reset", id, sizeof(sSecurityID)); }
void SecurityOrdinalEvent_FullResetFailed() { ClientLEDs_Set(0xF0F0F0F0, 0x00000000); LogEvent(Log_OrdinalEvent, "Invalid code for full reset (should be %s)", Security_GetUserFriendlySequence(SECURITY_FULL_RESET_SEQUENCE)); }
void SecurityOrdinalEvent_FullResetConfirmed() { ClientLEDs_Set(0x55555555, 0xAAAAAAAA); LogEvent(Log_OrdinalEvent, "Full reset confirmed by user"); }
void SecurityOrdinalEvent_RegistrationInitiated(sSecurityID const *id) { ClientLEDs_Set(0xF0F0F0F0, 0x00000000); LogEvent(Log_OrdinalEvent, "Accessor %Q initiates a new registration", id, sizeof(sSecurityID)); }
void SecurityOrdinalEvent_RegistrationSequence(uint32_t id) { ClientLEDs_Set(0x00030003, 0x03000300); LogEvent(Log_OrdinalEvent, "Please enter the registration sequence for governor %08uX", id); }
void SecurityOrdinalEvent_RegistrationNewKey(uint32_t id, sSecurityPayload const *new_key) { ClientLEDs_Set(0x00000000, 0xFF00FF00); LogEvent(Log_OrdinalEvent, "Governor %08uX sets initial key %Z", id, new_key, sizeof(sSecurityPayload)); }
void SecurityOrdinalEvent_ChallengeInitiated(sSecurityID const *accessor_id, uint32_t governor_id) { ClientLEDs_Set(0x80000000, 0x00008000); LogEvent(Log_OrdinalEvent, "Accessor %Q initiates a new challenge with governor %08uX", accessor_id, sizeof(sSecurityID), governor_id); }
void SecurityOrdinalEvent_ChallengeAttempt(uint32_t id) { ClientLEDs_Set(0x00000000, 0xAAAAAAAA); LogEvent(Log_OrdinalEvent, "Attempt to win the challenge from governor %08uX", id); }
void SecurityOrdinalEvent_ChallengeNewKey(uint32_t id, sSecurityPayload const *new_key) { ClientLEDs_Set(0x00030003, 0xFFFCFFFC); LogEvent(Log_OrdinalEvent, "Governor %08uX changes the key with %Z", id, new_key, sizeof(sSecurityPayload)); }
void SecurityOrdinalEvent_ChallengeNewKeyApproved(uint32_t id, sSecurityPayload const *new_key) { ClientLEDs_Set(0x00000000, 0xFCFCFCFC); LogEvent(Log_OrdinalEvent, "New key from governor %08uX checked and approved: %Z", id, new_key, sizeof(sSecurityPayload)); }
void SecurityOrdinalEvent_ChallengeWon(uint32_t id, bool action_success) { ClientLEDs_Set(action_success ? 0x00000000 : 0xFCFCFCFC, action_success ? 0xFCFCFCFC : 0x00000000); LogEvent(Log_OrdinalEvent, "Received won acknowledgment from governor %08uX with %s", id, action_success ? "success" : "failure"); }
void SecurityOrdinalEvent_AllDone() { ClientLEDs_Set(0x00000000, 0x00000000); LogEvent(Log_OrdinalEvent, "All done! Client power off" NL); DBG_Flush(); ClientPowerLock(false); while(true); }

void SecurityFail_NeverRegistered() { ClientLEDs_Set(0xFFFFFFFF, 0x00000000); LogEvent(Log_SecurityAlert, "Accessor never registered"); }
void SecurityFail_SessionWrongClient(sSecurityID const *id) { ClientLEDs_Set(0xAAAAAAAA, 0x00000000); LogEvent(Log_SecurityAlert, "Session to wrong accessor %Q", id, sizeof(sSecurityID)); }
void SecurityFail_SessionWrongServer(uint32_t id) { ClientLEDs_Set(0xAAAAAAAA, 0x00000000); LogEvent(Log_SecurityAlert, "Session from wrong governor %08uX", id); }
void SecurityFail_NoRegistrationForServer(uint32_t id) { LogEvent(Log_SecurityAlert, "Governor %08uX doesn't have registration on this accessor", id); }
void SecurityFail_ChallengeNewKeyFailed(uint32_t id, sSecurityPayload const *new_key) { ClientLEDs_Set(0xFCFCFCFC, 0x00000000); LogEvent(Log_SecurityAlert, "New key from governor %08uX failed to approve: %Z", id, new_key, sizeof(sSecurityPayload)); }
void SecurityFail_UnexpectedResponseForSession(uint32_t id) { LogEvent(Log_SecurityAlert, "Unexpected response for session from governor %08uX", id); }
void SecurityFail_UnexpectedCommand(uint32_t id, eSecurityCommand command) { LogEvent(Log_SecurityAlert, "Unexpected command %uX from governor %08uX", command, id); }
#endif

#ifdef SECURITY_SERVER
void SecuritySystemError_NoFreePipe(sSecurityID const *id) { LogEvent(Log_SystemError, "No free pipe for session requested from %Q", id, sizeof(sSecurityID)); }

void SecurityOrdinalEvent_ChallengeInitiated(sSecurityID const *id, eSecurityAction action, sSecurityPayload const *random) { Frames_PutFrameEx(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_ORDINALEVENT_CHALLENGE_INITIATED, 2, id, sizeof(sSecurityID), &action, sizeof(eSecurityAction)); LogEvent(Log_OrdinalEvent, "Accessor %Q initiates a new challenge for action %02X, random %Z", id, sizeof(sSecurityID), action, random, sizeof(sSecurityPayload)); }
void SecurityOrdinalEvent_ChallengeWonWithPrimaryKey(sSecurityID const *id, eSecurityAction action) { Frames_PutFrameEx(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_ORDINALEVENT_CHALLENGE_WON_WITH_PRIMARY_KEY, 2, id, sizeof(sSecurityID), &action, sizeof(eSecurityAction)); LogEvent(Log_OrdinalEvent, "Accessor %Q won the challenge for action %02X with primary key", id, sizeof(sSecurityID), action); }
void SecurityOrdinalEvent_ChallengeWonWithSecondaryKey(sSecurityID const *id, eSecurityAction action) { Frames_PutFrameEx(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_ORDINALEVENT_CHALLENGE_WON_WITH_SECONDARY_KEY, 2, id, sizeof(sSecurityID), &action, sizeof(eSecurityAction)); LogEvent(Log_OrdinalEvent, "Accessor %Q won the challenge for action %02X with secondary key and it will be set as primary", id, sizeof(sSecurityID), action); }
void SecurityOrdinalEvent_ChallengeKeyReplacement(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_ORDINALEVENT_CHALLENGE_KEY_REPLACEMENT, id, sizeof(sSecurityID)); LogEvent(Log_OrdinalEvent, "Governor initiates key replacement for accessor %Q", id, sizeof(sSecurityID)); }
void SecurityOrdinalEvent_ChallengeAnotherTryKeyReplacement(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_ORDINALEVENT_CHALLENGE_KEY_REPLACEMENT, id, sizeof(sSecurityID)); LogEvent(Log_OrdinalEvent, "Governor initiates another try of key replacement for accessor %Q", id, sizeof(sSecurityID)); }
void SecurityOrdinalEvent_RegistrationInitiated(sSecurityID const *id, uint32_t sequence, sSecurityPayload const *random) { Frames_PutFrameEx(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_ORDINALEVENT_REGISTRATION_INITIATED, 2, id, sizeof(sSecurityID), &sequence, sizeof(uint32_t)); LogEvent(Log_OrdinalEvent, "Accessor %Q initiates a new registration with sequence %s random %Z", id, sizeof(sSecurityID), Security_GetUserFriendlySequence(sequence), random, sizeof(sSecurityPayload)); }
void SecurityOrdinalEvent_RegistrationWonKeyReplacement(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_ORDINALEVENT_REGISTRATION_WON_KEY_REPLACEMENT, id, sizeof(sSecurityID)); LogEvent(Log_OrdinalEvent, "Accessor %Q won the registration challenge with sequence key and key replacement initiated", id, sizeof(sSecurityID)); }
void SecurityOrdinalEvent_RegistrationConfirmed(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_ORDINALEVENT_REGISTRATION_CONFIRMED, id, sizeof(sSecurityID)); LogEvent(Log_OrdinalEvent, "Accessor %Q confirmed the registration challenge with re-encoded key", id, sizeof(sSecurityID)); }

void SecurityFail_PipeAlreadyRequested(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_PIPE_ALREADY_REQUESTED, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Pipe already requested by accessor %Q", id, sizeof(sSecurityID)); }
void SecurityFail_CommunicationFailed(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_COMMUNICATION_FAILED, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Communication with accessor %Q failed", id, sizeof(sSecurityID)); }
void SecurityFail_SessionIsNotInitiated(uint8_t pipe) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_SESSION_IS_NOT_INITIATED, &pipe, sizeof(uint8_t)); LogEvent(Log_SecurityAlert, "Session from pipe %d is not initiated", pipe); }
void SecurityFail_SessionWrongClient(sSecurityID const *id, uint8_t pipe) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_SESSION_WRONG_ACCESSOR, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Session from wrong accessor %Q on pipe %d", id, sizeof(sSecurityID), pipe); }
void SecurityFail_SessionAlreadyFailed(sSecurityID const *id, uint8_t pipe) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_SESSION_ALREADY_FAILED, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Session from accessor %Q on pipe %d already failed", id, sizeof(sSecurityID), pipe); }
void SecurityFail_SessionTimeout(sSecurityID const *id, eSecurityClientChallengeContextState state) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_SESSION_TIMEOUT, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Session initiated by accessor %Q state %d timeout", id, sizeof(sSecurityID), state); }
void SecurityFail_UnexpectedResponseForSession(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_UNEXPECTED_RESPONSE_FOR_SESSION, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Unexpected response for session initiated by accessor %Q", id, sizeof(sSecurityID)); }
void SecurityFail_NoRegistrationForClient(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_NO_REGISTRATION_FOR_CLIENT, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Accessor %Q doesn't have registration on this governor", id, sizeof(sSecurityID)); }
void SecurityFail_NoProperActionForButtons(sSecurityID const *id, eSecurityActionButtons buttons) { Frames_PutFrameEx(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_NO_PROPER_ACTION_FOR_BUTTONS, 2, id, sizeof(sSecurityID), &buttons, sizeof(eSecurityActionButtons)); LogEvent(Log_SecurityAlert, "Accessor %Q doesn't have proper action for buttons %02uX", id, sizeof(sSecurityID), buttons); }
void SecurityFail_ClientIsNotActive(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_CLIENT_IS_NOT_ACTIVE, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Accessor %Q found, but is not active", id, sizeof(sSecurityID)); }
void SecurityFail_ChallengeLost(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_CHALLENGE_LOST, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Accessor %Q has lost the challenge", id, sizeof(sSecurityID)); }
void SecurityFail_RegistrationIsNotAllowed(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_REGISTRATION_NOT_ALLOWED, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Accessor %Q tries to register, but registration is not allowed", id, sizeof(sSecurityID)); }
void SecurityFail_RegistrationFromWrongClient(sSecurityID const *id1, sSecurityID const *id2) { Frames_PutFrameEx(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_REGISTRATION_FROM_WRONG_CLIENT, 2, id1, sizeof(sSecurityID), id2, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Registration allowed for accessor %Q, but received from client %Q", id1, sizeof(sSecurityID), id2, sizeof(sSecurityID)); }
void SecurityFail_RegistrationAlreadyExists(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_REGISTRATION_ALREADY_EXISTS, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Registration for accessor %Q already exists", id, sizeof(sSecurityID)); }
void SecurityFail_RegistrationLost(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_REGISTRATION_LOST, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Accessor %Q has lost the registration challenge", id, sizeof(sSecurityID)); }
void SecurityFail_RegistrationDenied(sSecurityID const *id) { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_REGISTRATION_DENIED, id, sizeof(sSecurityID)); LogEvent(Log_SecurityAlert, "Wrong confirmation from accessor %Q for registration challenge with re-encoded key", id, sizeof(sSecurityID)); }
void SecurityFail_RegistrationAllowanceTimeout() { Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_REGISTRATION_ALLOWANCE_TIMEOUT, nullptr, 0); LogEvent(Log_SecurityAlert, "Registration allowance timeout"); }
void SecurityFail_UnexpectedCommand(sSecurityID const *id, eSecurityCommand command) { Frames_PutFrameEx(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SECURITYFAIL_UNEXPECTED_COMMAND, 2, id, sizeof(sSecurityID), &command, sizeof(eSecurityCommand)); LogEvent(Log_SecurityAlert, "Unexpected command %uX from accessor %Q", command, id, sizeof(sSecurityID)); }

bool Security_PerformAction(eSecurityAction action) {
	eSecurityAction_Test _action = (eSecurityAction_Test)action;

	// TODO: Add procedures to perform actions
	DBG_PutFormat("===========================" NL "Performing action: %ud" NL "===========================" NL, _action);

	if(_action == SecurityAction_CloseDoor || _action == SecurityAction_OpenDoor || _action == SecurityAction_CloseDoorForced || _action == SecurityAction_OpenDoorForced) {
		bool _is_success = false; bool _is_forced = _action == SecurityAction_CloseDoorForced || _action == SecurityAction_OpenDoorForced;
		Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, _is_forced ? SEC_TO_PERIODIC(1100) : SEC_TO_PERIODIC(600), 0xAA, _action == SecurityAction_CloseDoor || _action == SecurityAction_CloseDoorForced ? 0x80 : 0x81, &_is_success, sizeof(bool), &_is_forced, sizeof(bool));
		return _is_success;
	}
	else if(_action == SecurityAction_DomophoneAutoOpener) {
		return true;
	}
	else return false;
}
#endif

void EtaRemoteControlKeyChainMainLoop(volatile bool *is_abort_requested, volatile bool *is_inited, void (*yield)()) {
	DBG_PutString(	"\nEta EtaRemoteControlKeyChain Firmware" NL
					" - Platform v " VER_PLATFORM "" NL
					" - Application v " VER_APP "" NL);

	// Power on stabilization timeout
	_delay_ms(100);

	while(true) {
		#ifdef SECURITY_SERVER
		Security_ServerInit(crc32(0, (void const *)0x1FFFF7AC, 12));
		#endif

		uint32_t _exti_tim = 0;

		#ifdef SECURITY_CLIENT
		ClientPowerLock(true);
		DBG_PutFormat("Battery voltage is: %W3udV" NL, ClientBaterryVoltage());
		Security_ClientInit();

//		SecurityOrdinalEvent_ButtonsPressed(0);
//		SecurityOrdinalEvent_GeneratedNewID(&__id_free);
//		SecurityOrdinalEvent_FullResetInitiated(&__id_free);
//		SecurityOrdinalEvent_FullResetFailed();
//		SecurityOrdinalEvent_FullResetConfirmed();
//		SecurityOrdinalEvent_RegistrationInitiated(&__id_free);
//		SecurityOrdinalEvent_RegistrationSequence(0xAA55AA55);
//		SecurityOrdinalEvent_RegistrationNewKey(0xAA55AA55, &__key_free);
//		SecurityOrdinalEvent_ChallengeInitiated(&__id_free, 0xAA);
//		SecurityOrdinalEvent_ChallengeNewKey(0xAA55AA55, &__key_free);
//		SecurityOrdinalEvent_ChallengeNewKeyApproved(0xAA55AA55, &__key_free);
//		SecurityOrdinalEvent_ChallengeWon(0xAA55AA55);
//
//		SecurityFail_NeverRegistered();
//		SecurityFail_SessionWrongClient(&__id_free);
//		SecurityFail_SessionWrongServer(0xAA55AA55);
//		SecurityFail_NoRegistrationForServer(0xAA55AA55);
//		SecurityFail_ChallengeNewKeyFailed(0xAA55AA55, &__key_free);
//		SecurityFail_UnexpectedResponseForSession(0xAA55AA55);
//		SecurityFail_UnexpectedCommand(0xAA55AA55, 0x77);
		#endif

		while(true) {
			// Watchdog reset
			//IWDG_ReloadCounter();

			/// *********************************************************************************************************
			/// *********************************************************************************************************
			/// *********************************************************************************************************
			/// *********************************************************************************************************

			#ifdef SECURITY_SERVER
			Security_ServerPoll(&TimerMs);
			#endif

			/// *********************************************************************************************************
			/// *********************************************************************************************************
			/// *********************************************************************************************************
			/// *********************************************************************************************************

			#ifdef SECURITY_CLIENT
			Security_ClientPoll(&TimerMs, (eSecurityActionPressButtons *)&SecurityActionButtons);
			#endif

			if((TimerMs - _exti_tim) > SEC_TO_PERIODIC(1000)) {
				_exti_tim += SEC_TO_PERIODIC(1000);
				//DBG_PutFormat(" ### Encoder counter %d" NL, EncoderCounter);
			}

			/// *********************************************************************************************************
			/// *********************************************************************************************************
			/// *********************************************************************************************************
			/// *********************************************************************************************************

			#ifdef SECURITY_SERVER
			if(Frames_IsFrameAvailable(USART_DBG_portparamlast)) {
				uint8_t _frame_addr = Frames_GetFrameAddress(USART_DBG_portparamlast);
				if(_frame_addr == FRAME_ADDRESS) {
					sFrameCmd _frame_cmd = (sFrameCmd)Frames_GetFrameCommand(USART_DBG_portparamlast);
					uint8_t _frame_size = Frames_GetFrameSize(USART_DBG_portparamlast);
					uint8_t _frame_data[_frame_size];
					Frames_GetFrameData(USART_DBG_portparam _frame_data, 0, _frame_size); Frames_RemoveFrame(USART_DBG_portparamlast);

					if(_frame_cmd == FRAME_CMD_GET_SERVER_INFO && !_frame_size) {
						sFrame_GetServerInfo_Response _response = { .MaxClientRegistersCount = SECURITY_SERVER_DISK_CLIENT_REGISTERS_COUNT };
						Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_GET_SERVER_INFO, &_response, sizeof(sFrame_GetServerInfo_Response));
					}
					else if(_frame_cmd == FRAME_CMD_GET_NEXT_CLIENT_REGISTER && _frame_size == sizeof(sFrame_GetNextClientRegister_Request)) {
						static uint32_t _iterrator_index = 0;
						sFrame_GetNextClientRegister_Request const *_request = (sFrame_GetNextClientRegister_Request const *)_frame_data;
						if(_request->StartFromBeginning) _iterrator_index = 0;
						sSecurityClientRegister const *_client_register = Security_ClientRegister_GetValidStartingFromIndex(&_iterrator_index);
						Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_GET_NEXT_CLIENT_REGISTER, _client_register, _client_register ? sizeof(sSecurityClientRegister) : 0);
					}
					else if(_frame_cmd == FRAME_CMD_GET_CLIENT_REGISTER && _frame_size == sizeof(sFrame_ClientRegister_Request)) {
						sFrame_ClientRegister_Request const *_request = (sFrame_ClientRegister_Request const *)_frame_data;
						sSecurityClientRegister const *_client_register = Security_ClientRegister_GetByID(&_request->ID);
						Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_GET_CLIENT_REGISTER, _client_register, _client_register ? sizeof(sSecurityClientRegister) : 0);
					}
					else if(_frame_cmd == FRAME_CMD_SET_CLIENT_REGISTER && _frame_size == sizeof(sFrame_ClientRegisterData_Request)) {
						sFrame_ClientRegisterData_Request const *_request = (sFrame_ClientRegisterData_Request const *)_frame_data;
						bool _res = false;
						sSecurityClientRegister const *_client_register_flash = Security_ClientRegister_GetByID(&_request->ID);
						if(_client_register_flash) {
							sSecurityClientRegister _client_register_new = *_client_register_flash;
							eSecurityAction const *_action_flash = _client_register_flash->Actions; eSecurityAction const *_action_new = _request->Actions; eSecurityAction *_action_dst = _client_register_new.Actions;
							for(uint32_t _idx = 0; _idx < SECURITY_CLIENT_ACTION_ENTRIES_COUNT; _idx++) {
								*_action_dst = *_action_new == SecurityAction_None && *_action_flash == SecurityAction_Free ? SecurityAction_Free : *_action_new;
								_action_flash++; _action_new++; _action_dst++;
							}
							char const *_description_flash = _client_register_flash->Description; char const *_description_new = _request->Description; char *_description_dst = _client_register_new.Description;
							for(uint32_t _idx = 0; _idx < SECURITY_CLIENT_DESCRIPTION_COUNT; _idx++) {
								*_description_dst = *_description_new == 0 && *_description_flash == 0xFF ? 0xFF : *_description_new;
								_description_flash++; _description_new++; _description_dst++;
							}
							_res = Security_ClientRegister_Modify(&_client_register_flash, &_client_register_new);
						}
						Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SET_CLIENT_REGISTER, &_res, sizeof(bool));
					}
					else if(_frame_cmd == FRAME_CMD_CLOBBER_CLIENT_REGISTER && _frame_size == sizeof(sFrame_ClientRegister_Request)) {
						bool _res = false;
						sFrame_ClientRegister_Request const *_request = (sFrame_ClientRegister_Request const *)_frame_data;
						sSecurityClientRegister const *_client_register = Security_ClientRegister_GetByID(&_request->ID);
						if(_client_register) { _res = true; Security_ClientRegister_Clobber(_client_register); }
						Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_CLOBBER_CLIENT_REGISTER, &_res, sizeof(bool));
					}
					else if(_frame_cmd == FRAME_CMD_SET_CLIENT_REGISTER_ACTIVE && _frame_size == sizeof(sFrame_SetClientRegisterActive_Request)) {
						bool _res = false;
						sFrame_SetClientRegisterActive_Request const *_request = (sFrame_SetClientRegisterActive_Request const *)_frame_data;
						sSecurityClientRegister const *_client_register = Security_ClientRegister_GetByID(&_request->ID);
						if(_client_register) { _res = Security_ClientRegister_IsActive(_client_register) != _request->IsActive ? Security_ClientRegister_ToggleActive(&_client_register) : true; }
						Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, FRAME_CMD_SET_CLIENT_REGISTER_ACTIVE, &_res, sizeof(bool));
					}
					else if(_frame_cmd == FRAME_CMD_ALLOW_REGISTRATION && _frame_size == sizeof(sFrame_ClientRegister_Request)) {
						sFrame_ClientRegister_Request const *_request = (sFrame_ClientRegister_Request const *)_frame_data;
						if(memcmp(&_request->ID, &__id_clobbered, sizeof(sSecurityID))) {
							sSecurityClientRegister const *_client_register = Security_ClientRegister_GetByID(&_request->ID);
							if(_client_register) Security_AllowRegistration(&TimerMs, &_client_register->ID);
						}
						else Security_AllowRegistration(&TimerMs, nullptr);
					}
					else if(_frame_cmd == FRAME_CMD_GET_CLIENT_REGISTERING && !_frame_size) {




						sFrame_ClientRegister_Request const *_request = (sFrame_ClientRegister_Request const *)_frame_data;
						if(memcmp(&_request->ID, &__id_clobbered, sizeof(sSecurityID))) {
							sSecurityClientRegister const *_client_register = Security_ClientRegister_GetByID(&_request->ID);
							if(_client_register) Security_AllowRegistration(&TimerMs, &_client_register->ID);
						}
						else Security_AllowRegistration(&TimerMs, nullptr);
					}
					else if(_frame_cmd == FRAME_CMD_TEST_RFINFO && !_frame_size) {
						DBG_PutString("FRAME_CMD_TEST_RFINFO" NL);
						sRF_ConfigInfo _cfg_info;
						RF_GetConfigInfo(&RFMSPIServer, &_cfg_info);
						DBG_PutFormat(" STATUS      %08b" NL, _cfg_info.STATUS);
						DBG_PutFormat(" FIFO_STATUS %08b" NL, _cfg_info.FIFO_STATUS);
						DBG_PutFormat(" CONFIG      %08b" NL, _cfg_info.Settings.CONFIG);
						DBG_PutFormat(" ENAA        %08b" NL, _cfg_info.Settings.ENAA);
						DBG_PutFormat(" EN_RX_ADDR  %08b" NL, _cfg_info.Settings.EN_RX_ADDR);
						DBG_PutFormat(" SETUP_AW    %08b" NL, _cfg_info.Settings.SETUP_AW);
						DBG_PutFormat(" SETUP_RETR  %08b" NL, _cfg_info.Settings.SETUP_RETR);
						DBG_PutFormat(" RF_CH       %d" NL, _cfg_info.Settings.RF_CH);
						DBG_PutFormat(" RF_SETUP    %08b" NL, _cfg_info.Settings.RF_SETUP);
						DBG_PutFormat(" FEATURE     %08b" NL, _cfg_info.Settings.FEATURE);
						DBG_PutFormat(" RX_ADDR_P0  %Z" NL, _cfg_info.Settings.RX_ADDR_P0, 5);
						DBG_PutFormat(" RX_ADDR_P1  %Z" NL, _cfg_info.Settings.RX_ADDR_P1, 5);
						DBG_PutFormat(" RX_ADDR_P2  %02X" NL, _cfg_info.Settings.RX_ADDR_P2);
						DBG_PutFormat(" RX_ADDR_P3  %02X" NL, _cfg_info.Settings.RX_ADDR_P3);
						DBG_PutFormat(" RX_ADDR_P4  %02X" NL, _cfg_info.Settings.RX_ADDR_P4);
						DBG_PutFormat(" RX_ADDR_P5  %02X" NL, _cfg_info.Settings.RX_ADDR_P5);
						DBG_PutFormat(" RX_PW_P0    %d" NL, _cfg_info.Settings.RX_PW_P0);
						DBG_PutFormat(" RX_PW_P1    %d" NL, _cfg_info.Settings.RX_PW_P1);
						DBG_PutFormat(" RX_PW_P2    %d" NL, _cfg_info.Settings.RX_PW_P2);
						DBG_PutFormat(" RX_PW_P3    %d" NL, _cfg_info.Settings.RX_PW_P3);
						DBG_PutFormat(" RX_PW_P4    %d" NL, _cfg_info.Settings.RX_PW_P4);
						DBG_PutFormat(" RX_PW_P5    %d" NL, _cfg_info.Settings.RX_PW_P5);
						DBG_PutFormat(" DYNPD       %08b" NL, _cfg_info.DYNPD);
					}
					else if(_frame_cmd == FRAME_CMD_TEST_EEPROM_PAGES_WRITE && !_frame_size) {
						DBG_PutString("FRAME_CMD_TEST_EEPROM_PAGES_WRITE" NL);
						uint32_t _rnd[4];
						for(uint8_t _addr = 0; _addr < 0x10; _addr++) {
							DBG_PutFormat(" Writing to unit %2d" NL, _addr);
							Security_RandomFill(_rnd, 4); DBG_PutFormat("  block - 1: %s" NL, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(170), 0xB0 | _addr, 0x00, nullptr, 0, _rnd, 16) ? "true" : "false");
							Security_RandomFill(_rnd, 4); DBG_PutFormat("  block - 2: %s" NL, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(170), 0xB0 | _addr, 0x10, nullptr, 0, _rnd, 16) ? "true" : "false");
							Security_RandomFill(_rnd, 4); DBG_PutFormat("  block - 3: %s" NL, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(170), 0xB0 | _addr, 0x20, nullptr, 0, _rnd, 16) ? "true" : "false");
							Security_RandomFill(_rnd, 4); DBG_PutFormat("  block - 4: %s" NL, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(170), 0xB0 | _addr, 0x30, nullptr, 0, _rnd, 16) ? "true" : "false");
						}
					}
					else if(_frame_cmd == FRAME_CMD_TEST_EEPROM_PAGES_READ && !_frame_size) {
						_read_pages:;
						uint8_t _frame_data[16];
						DBG_PutString("FRAME_CMD_TEST_EEPROM_PAGES_READ" NL);
						for(uint8_t _addr = 0; _addr < 0x10; _addr++) {
							DBG_PutFormat(" Reading from unit %2d" NL, _addr);
							DBG_PutString("  block - 1: "); if(Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(120), 0xB0 | _addr, 0x01, _frame_data, sizeof_array(_frame_data), nullptr, 0)) DBG_PutFormat("%Z" NL, _frame_data, sizeof_array(_frame_data)); else DBG_PutString("failed" NL);
							DBG_PutString("  block - 2: "); if(Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(120), 0xB0 | _addr, 0x11, _frame_data, sizeof_array(_frame_data), nullptr, 0)) DBG_PutFormat("%Z" NL, _frame_data, sizeof_array(_frame_data)); else DBG_PutString("failed" NL);
							DBG_PutString("  block - 3: "); if(Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(120), 0xB0 | _addr, 0x21, _frame_data, sizeof_array(_frame_data), nullptr, 0)) DBG_PutFormat("%Z" NL, _frame_data, sizeof_array(_frame_data)); else DBG_PutString("failed" NL);
							DBG_PutString("  block - 4: "); if(Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(120), 0xB0 | _addr, 0x31, _frame_data, sizeof_array(_frame_data), nullptr, 0)) DBG_PutFormat("%Z" NL, _frame_data, sizeof_array(_frame_data)); else DBG_PutString("failed" NL);
						}
					}
					else if(_frame_cmd == FRAME_CMD_TEST_EEPROM_ERASE && !_frame_size) {
						DBG_PutString("FRAME_CMD_TEST_EEPROM_ERASE" NL);
						for(uint8_t _addr = 0; _addr < 0x10; _addr++) {
							DBG_PutFormat(" Erasing unit %2d" NL, _addr);
							DBG_PutFormat("  block - 1: %s" NL, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(170), 0xB0 | _addr, 0x00, nullptr, 0, __id_free.ID, 16) ? "true" : "false");
							DBG_PutFormat("  block - 2: %s" NL, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(170), 0xB0 | _addr, 0x10, nullptr, 0, __id_free.ID, 16) ? "true" : "false");
							DBG_PutFormat("  block - 3: %s" NL, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(170), 0xB0 | _addr, 0x20, nullptr, 0, __id_free.ID, 16) ? "true" : "false");
							DBG_PutFormat("  block - 4: %s" NL, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(170), 0xB0 | _addr, 0x30, nullptr, 0, __id_free.ID, 16) ? "true" : "false");
						}
						goto _read_pages;
					}
					else if(_frame_cmd == FRAME_CMD_TEST_GET_ADC && !_frame_size) {
						DBG_PutString("FRAME_CMD_TEST_GET_ADC" NL);
						uint16_t _adc;
						for(uint8_t _addr = 0; _addr < 0x10; _addr++) {
							DBG_PutFormat(" ADC of unit %2d: ", _addr);
							if(Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(60), 0xB0 | _addr, 0x02, &_adc, sizeof(_adc), nullptr, 0)) {
								DBG_PutFormat("raw: %04uX: voltage: %W3" NL, _adc, 1100 * 133 / 33 * _adc / 1023);
							}
							else DBG_PutString("failed" NL);
						}
					}
					else if(_frame_cmd == FRAME_CMD_TEST_SWITCH_SINK && !_frame_size) {
						DBG_PutString("FRAME_CMD_TEST_SWITCH_SINK" NL);
						for(uint8_t _value = 0; _value < 4; _value++) {
							DBG_PutFormat(" Value is %d: " NL, _value);
							uint8_t _vvv = _value << 3;
							for(uint8_t _addr = 0; _addr < 0x10; _addr++) { DBG_PutFormat("  Testing unit %2d: %s" NL, _addr, Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(50), 0xB0 | _addr, 0x03, nullptr, 0, &_vvv, 1) ? "true" : "false"); }
						}
					}
					else if(_frame_cmd == FRAME_CMD_TEST_LOCK && _frame_size == 1) {
						DBG_PutString("FRAME_CMD_TEST_LOCK" NL);
						bool _is_success = false;
						DBG_PutString(" Locking.....");
						if(Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(1100), 0xAA, 0x80, &_is_success, sizeof(_is_success), _frame_data, 1)) { DBG_PutFormat("%s" NL, _is_success ? "Done" : "Fault"); }
						else DBG_PutString("failed" NL);
					}
					else if(_frame_cmd == FRAME_CMD_TEST_UNLOCK && _frame_size == 1) {
						DBG_PutString("FRAME_CMD_TEST_UNLOCK" NL);
						bool _is_success = false;
						DBG_PutString(" Unlocking.....");
						if(Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(1100), 0xAA, 0x81, &_is_success, sizeof(_is_success), _frame_data, 1)) { DBG_PutFormat("%s" NL, _is_success ? "Done" : "Fault"); }
						else DBG_PutString("failed" NL);
					}
					else if(_frame_cmd == FRAME_CMD_TEST_GETLOCKSTATUS && !_frame_size) {
						DBG_PutString("FRAME_CMD_TEST_UNLOCK" NL);
						uint8_t _state = false;
						DBG_PutString(" GetStatus.....");
						if(Frames_RequestResponseWithConstSize(USART_BCELL_portparam &TimerMs, SEC_TO_PERIODIC(50), 0xAA, 0x82, &_state, sizeof(_state), nullptr, 0)) { DBG_PutFormat("%d" NL, _state); }
						else DBG_PutString("failed" NL);
					}
				}
			}
			if(Frames_IsTransmitterFree(USART_DBG_portparamlast)) {
//				Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, (uint8_t)FRAME_CMD_STATUS, &_status, sizeof(sStatus));
			}

			if(Frames_IsFrameAvailable(USART_BCELL_portparamlast)) {
				sFrameCmd _frame_cmd = (sFrameCmd)Frames_GetFrameCommand(USART_BCELL_portparamlast);
				uint8_t _frame_size = Frames_GetFrameSize(USART_BCELL_portparamlast);
				uint8_t _frame_data[_frame_size];

				Frames_GetFrameData(USART_BCELL_portparam _frame_data, 0, _frame_size); Frames_RemoveFrame(USART_BCELL_portparamlast);
				DBG_PutFormat(" BCell frame: cmd %02uX: %Z" NL, _frame_cmd, _frame_data, _frame_size);
			}
			if(Frames_IsTransmitterFree(USART_BCELL_portparamlast)) {
//				Frames_PutFrame(USART_DBG_portparam FRAME_ADDRESS, (uint8_t)FRAME_CMD_STATUS, &_status, sizeof(sStatus));
			}
			#endif

			/// *********************************************************************************************************
			/// *********************************************************************************************************
			/// *********************************************************************************************************
			/// *********************************************************************************************************

			PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
		}
		DBG_PutString("=====================================" NL "All over again" NL "=====================================" NL);
	}



//	RF_Mode(RFM1, RFMMode_Receive);
//	//RF_TransmitterAddress(RFM2, RF_Settings.TX_ADDR_RX_ADDR_P0);
//	RF_Mode(RFM2, RFMMode_Transmit);
//
//	uint32_t _cnt = 0;
//	while(true) {
//		DBG_PutFormat("cnt new: %d" NL, _cnt++);
//		bool _is_transmitted = RF_PackageTransmit(RFM2, false, __test_string, 32);
//		DBG_PutFormat("Is Transmitted: %s" NL, _is_transmitted ? "yes" : "no");
//		DBG_PutFormat("Is Receiver buffer empty: %s" NL, RF_IsReceiverBufferEmpty(RFM1) ? "yes" : "no");
//		if(_is_transmitted) {
//			uint8_t _len_recv;
//			uint8_t _pipe_recv = RF_PackageReceive(RFM1, __test_buf, &_len_recv); __test_buf[_len_recv] = 0;
//			DBG_PutFormat("Receiver packet pipe: %ud, len %ud, text %s" NL, _pipe_recv, _len_recv, __test_buf);
//		}
//		else {
//			_delay_ms(500);
//		}
//	}
//
//	//RF_Mode(RFM1, RFMMode_PowerDown); RF_Mode(RFM2, RFMMode_PowerDown);
//
//	DBG_PutString("Test over" NL);

}
