/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : main.c
 ***********************************************************************/

#include "main.h"
#include <stdbool.h>
#include <string.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "eta_crc.h"
#include "eta_usart.h"
#include "eta_dbg.h"

typedef struct {
	uint16_t uart_div;
	uint32_t sd_current_sector;
} sUL_State;

static sUL_State EEMEM eeprom_state;
static sUL_State state;

static bool no_error;
static bool mode;
static uint32_t sd_bytes_written;
static uint16_t sd_sector_bytes_written;
static eUL_SDCardType sd_cardtype;

static void spi_cs(bool enable) { if(enable) SPI_SDCS_PORT &= ~(1 << SPI_SDCS_PIN); else SPI_SDCS_PORT |= (1 << SPI_SDCS_PIN); }
static uint8_t spi_readwrite(uint8_t data) {
	LED_SD_PORT |= (1 << LED_SD_PIN);
	uint8_t _res = 0;
	for(uint8_t _i = 8; _i; _i--) {
		_res <<= 1;
		if(data & 0x80) SPI_MOSI_PORT |= (1 << SPI_MOSI_PIN); else SPI_MOSI_PORT &= ~(1 << SPI_MOSI_PIN);
		SPI_SCK_PORT |= (1 << SPI_SCK_PIN);
		data <<= 1;
		_res |= ((SPI_MISO_PORTPIN >> SPI_MISO_PIN) & 0x01);
		SPI_SCK_PORT &= ~(1 << SPI_SCK_PIN);
	}
	LED_SD_PORT &= ~(1 << LED_SD_PIN);
	return _res;
}

static bool sd_wait_not_busy() {
	uint16_t _retries = 10000;
	while(spi_readwrite(0xFF) != 0xFF && _retries--);
	return _retries;
}
static uint8_t sd_wait_token() {
	uint16_t _tries = 40000; uint8_t _res;
	while((_res = spi_readwrite(0xFF)) == 0xFF && _tries--);
	return _res;
}
static uint8_t sd_cmd(eUL_SDCommand cmd, uint32_t value) {
	uint8_t _res;
	sd_wait_not_busy();
	if(cmd & 0x80) { sd_cmd(SDCommand_CMD55, 0); cmd &= 0x7F; }
	spi_readwrite(cmd);
	spi_readwrite(value >> 24);
	spi_readwrite(value >> 16);
	spi_readwrite(value >> 8);
	spi_readwrite(value);
	uint8_t _crc = cmd == SDCommand_CMD0 ? 0x95 : cmd == SDCommand_CMD8 ? 0x87 : 1;
	spi_readwrite(_crc);
	while((_res = spi_readwrite(0xFF)) & 0x80);
	return _res;
}
static bool sd_cmd_wait(eUL_SDCommand cmd, uint32_t value) {
	uint16_t _retries = 10000;
	while(sd_cmd(cmd, value) && _retries--);
	return _retries;
}
static uint32_t sd_recieve32() { return make_int32_be(spi_readwrite(0xFF), spi_readwrite(0xFF), spi_readwrite(0xFF), spi_readwrite(0xFF)); }

static bool sd_init() {
	// GO_IDLE_STATE
	if(sd_cmd(SDCommand_CMD0, 0) == 1) {
		// Check for SDv2
		if(sd_cmd(SDCommand_CMD8, 0x1AA) == 1) {
			if((((uint16_t)sd_recieve32()) & 0xFFF) == 0x1AA) {
				if(sd_cmd_wait(SDCommand_ACMD41, (1UL << 30))) {
					if(!sd_cmd(SDCommand_CMD58, 0)) {
						sd_cardtype = (sd_recieve32() & (1UL << 30)) ? SDCardType_SD2 | SDCardType_CT_BLOCK : SDCardType_SD2;
					}
				}
			}
		}
		// Card SDv1 or MMC
		else {
			eUL_SDCommand _cmd_next;
			if(sd_cmd(SDCommand_ACMD41, 0) <= 1) { sd_cardtype = SDCardType_SD1; _cmd_next = SDCommand_ACMD41; }
			else { sd_cardtype = SDCardType_MMC; _cmd_next = SDCommand_CMD1; }
			if(!sd_cmd_wait(_cmd_next, 0) || sd_cmd(SDCommand_CMD16, 512)) sd_cardtype = 0;
		}
	}
	return (no_error = (bool)sd_cardtype);
}
static bool sd_read_sector(uint32_t sector) {
	if(!(sd_cardtype & SDCardType_CT_BLOCK)) sector <<= 9;
	if(!sd_cmd(SDCommand_CMD17, sector)) {
		if(sd_wait_token() == 0xFE) {
			uint16_t _bytes_left = 512;
			while(_bytes_left--) { usart_put_char(spi_readwrite(0xFF)); }
			return (no_error = true);
		}
	}
	return (no_error = false);
}
static bool sd_write_init(uint32_t sector) {
	if(!(sd_cardtype & SDCardType_CT_BLOCK)) sector <<= 9;
	if(!sd_cmd(SDCommand_CMD24, sector)) {
		sd_sector_bytes_written = 0;
		spi_readwrite(0xFE);
		return (no_error = true);
	}
	return (no_error = false);
}
static bool sd_write_fin() {
	uint16_t _dummy = 514 - sd_sector_bytes_written;
	while(_dummy--) { spi_readwrite(0x00); }
	if((spi_readwrite(0xFF) & 0x1F) == 0x05) { return sd_wait_not_busy(); }
	return (no_error = false);
}
static bool sd_write_byte(uint8_t byte) {
	spi_readwrite(byte);
	sd_bytes_written++; sd_sector_bytes_written++;
	if(sd_sector_bytes_written == 512) { return sd_write_fin() && sd_write_init(++state.sd_current_sector); }
	else return (no_error = true);
}
static void sd_int_write_byte(uint8_t byte) { spi_readwrite(byte); }
static void sd_int_write_dword(uint32_t dword) { sd_int_write_byte(dword); sd_int_write_byte(dword >> 8); sd_int_write_byte(dword >> 16); sd_int_write_byte(dword >> 24); }

static void init() {
	usartinit_init();

	PORTB = 0x16; DDRB = 0x0E; PORTD = 0x40; DDRD = 0x3C;

	// Enable Timer0 overflow
	TCCR0 = (3 << CS00); TIMSK = (1 << TOIE0);

	sei();

	wdt_enable(4);
}

uint8_t volatile global_ticks;
bool volatile global_blink;
ISR(TIMER0_OVF0_vect) {
	global_ticks++; if(!global_ticks) global_blink = !global_blink;
	if(mode || global_blink) LED_MODE_PORT |= (1 << LED_MODE_PIN); else LED_MODE_PORT &= ~(1 << LED_MODE_PIN);
	if(no_error) LED_ERROR_PORT &= ~(1 << LED_ERROR_PIN); else LED_ERROR_PORT |= (1 << LED_ERROR_PIN);
}

__attribute__((OS_main))
void main() {
	init();
	eeprom_read_block(&state, &eeprom_state, sizeof(sUL_State));

	for(uint8_t _i = 10; _i; _i--) spi_readwrite(0xFF);
	spi_cs(true);
	sd_init();
	usart_put_char(sd_cardtype);

	for(uint16_t volatile _i = 50000; _i; _i--);

	bool _btn_mode_prev = false;
	while(true) {
		wdt_reset();
		if(!no_error) mode = false;

		// Logging mode
		if(mode) {
			if(usart_is_char_available()) {
				LED_ACTIVITY_PORT |= (1 << LED_ACTIVITY_PIN);
				sd_write_byte(usart_get_char());
				LED_ACTIVITY_PORT &= ~(1 << LED_ACTIVITY_PIN);
			}
		}
		// Config mode
		else {
			if(usart_is_char_available()) {
				char _cmd = usart_get_char();
				// Erase
				if(_cmd == 'e') { if((uint8_t)usart_get_char() == 0xAA) eeprom_write_dword(&eeprom_state.sd_current_sector, state.sd_current_sector = 0); }
				// Read sector
				else if(_cmd == 'r') { sd_read_sector(make_int32(usart_get_char(), usart_get_char(), usart_get_char(), usart_get_char())); }
				// Get status
				else if(_cmd == 's') { usart_put_block(&state, sizeof(sUL_State)); }
				// Set baud rate
				else if(_cmd == 'b') { if((uint8_t)usart_get_char() == 0xAA) eeprom_write_word(&eeprom_state.uart_div, state.uart_div = make_int16(usart_get_char(), usart_get_char())); }
				// Reset
				else if(_cmd == 'z') __asm("ldi r30, 0\t\n ldi r31, 0\t\n ijmp\t\n");
			}
		}

		if(no_error) {
			bool _btn_mode = !(BTN_MODE_PORTPIN & (1 << BTN_MODE_PIN));
			if(_btn_mode_prev != _btn_mode) {
				_btn_mode_prev = _btn_mode;
				if(_btn_mode) {
					mode = !mode;
					// Logging mode
					if(mode) {
						USARTINIT_UBRR = state.uart_div;
						sd_bytes_written = 0; state.sd_current_sector++;
						sd_write_init(state.sd_current_sector);
					}
					// Config mode
					else {
						USARTINIT_UBRR = 3; // ((F_CPU / 16 / USART_BAUDRATE) - 1) = 125000 for 8MHz
						if(sd_write_fin()) {
							if(sd_bytes_written) {
								state.sd_current_sector -= ((sd_bytes_written >> 9) + 1);
								if(sd_write_init(state.sd_current_sector)) {
									sd_int_write_byte(0xAA); sd_int_write_byte(0x55); sd_int_write_byte(0xAA); sd_int_write_byte(0x55); sd_int_write_dword(sd_bytes_written);
									*((uint8_t *)&sd_sector_bytes_written) = 8;
									if(sd_write_fin()) {
										state.sd_current_sector += ((sd_bytes_written + 511) >> 9) + 1;
										eeprom_write_dword(&eeprom_state.sd_current_sector, state.sd_current_sector);
									}
								}
							}
							else state.sd_current_sector--;
						}
					}
				}
			}
		}
	}

	__builtin_unreachable();
}
