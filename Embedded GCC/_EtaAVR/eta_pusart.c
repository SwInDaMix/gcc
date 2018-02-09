/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_pusart.c
 ***********************************************************************/

#include <avr/interrupt.h>
#include "eta_pusart.h"

/// ********************************************************************
/// PUSART Generics
/// ********************************************************************

#if (defined PUSART_RECEIVER || defined PUSART_TRANSMITTER)
typedef struct {
#ifdef PUSART_RECEIVER
	uint16_t rxvalue_ready;
	pusart_value_t rxvalue;
	uint8_t rxbitcount;
#endif
#ifdef PUSART_TRANSMITTER
	uint16_t txbyte;
	uint8_t txbitcount;
#endif
#if defined PUSART_8BITCOUNTERS && (defined PUSART_TIMER_COUNTER || defined PUSART_IDLE_COUNTER)
	uint8_t divtimer;
#endif
#ifdef PUSART_TIMER_COUNTER
	pusart_counter_t timer;
#endif
#ifdef PUSART_IDLE_COUNTER
	pusart_counter_t idle_counter;
#endif
} pusart_ctx_t;
static volatile pusart_ctx_t pusart_ctx;

#ifdef PUSART_TIMER_COUNTER
pusart_counter_t pusart_timer_get() {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	#ifdef PUSART_TIMER_COUNTER
	return _ctx->timer;
	#else
	cli(); pusart_counter_t _ret = _ctx->timer; sei();
	return _ret;
	#endif
}
#endif
#ifdef PUSART_IDLE_COUNTER
pusart_counter_t pusart_idle_counter_get() {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	#ifdef PUSART_TIMER_COUNTER
	return _ctx->idle_counter;
	#else
	cli(); pusart_counter_t _ret = _ctx->idle_counter; sei();
	return _ret;
	#endif
}
#endif
#endif

/// ********************************************************************
/// PUSART Receiver Capability
/// ********************************************************************

#ifdef PUSART_RECEIVER
ISR(TIM0_COMPB_vect) {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	if(PUSART_RXPORT & (1 << PUSART_RXPIN)) _ctx->rxvalue |= (1 << (PUSART_BITS - 1));	// ��������� � ����� ��������� ���� RX, e��� � 1, �� ����� 1 � ������� ������ rxvalue
	if(!(--_ctx->rxbitcount)) {															// ��������� �� 1 ������� ��� � ��������� �� ���� �� �� �����
		TIMSK0 &= ~(1 << OCIE0B);														// ���� ��, ��������� ���������� TIM0_COMPB
		TIFR0 = (1 << OCF0B);															// ������� ���� ���������� TIM0_COMPB
		GIFR = (1 << INTF0);															// ������� ���� ���������� �� INT0
		GIMSK |= (1 << INT0);															// ��������� ���������� INT0
		_ctx->rxvalue_ready = _ctx->rxvalue | 0x8000;									// �������� ��������� �������� ��� ������� � rxvalue_ready
	}
	else { _ctx->rxvalue >>= 1; }														// ����� �������� rxvalue ������ �� 1
}
ISR(INT0_vect) {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	MCUCR |= (2 << ISC00); GIFR = (1 << INTF0);
	_ctx->rxbitcount = 1 + PUSART_BITS;													// 1 ��������� ��� � ���������� ��� ������
	_ctx->rxvalue = 0;																	// �������� ���������� rxvalue
	uint8_t _cnt = TCNT0;
	if(_cnt < (PUSART_TIMBAUD >> 1)) { OCR0B = _cnt + (PUSART_TIMBAUD >> 1); }			// ���� ������ �� �������� �� �������� �������� �������, �� ���������� ���������� � ������� ������� ������ ��� �������
	else { OCR0B = _cnt - (PUSART_TIMBAUD >> 1); }										// ����� ���������� ���������� ��� � ��������� ������� �������
	GIMSK &= ~(1 << INT0);																// ��������� ���������� �� INT0
	TIFR0 = (1 << OCF0B);												// ������� ����� ���������� TIM0_COMPA (B)
	TIMSK0 |= (1 << OCIE0B);															// ��������� ���������� �� OCR0B
	#ifdef PUSART_IDLE_COUNTER
	_ctx->idle_counter = 0;
	#endif
}

bool pusart_is_value_available() {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	return _ctx->rxvalue_ready & 0x8000;
}
pusart_value_t pusart_get_value() {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	while(!(_ctx->rxvalue_ready & 0x8000));
	cli();
	pusart_value_t _value = (pusart_value_t)_ctx->rxvalue_ready; _ctx->rxvalue_ready = 0;
	sei();
	return _value;
}
#endif

/// ********************************************************************
/// PUSART Transmitter Capability
/// ********************************************************************

#ifdef PUSART_TRANSMITTER
void pusart_put_value(pusart_value_t value) {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	while(_ctx->txbitcount);										// ���� ���� ���������� �������� �������� �����
	uint16_t _txbyte = (value | ~((1 << PUSART_BITS) - 1)) << 1;	// ����� � ������� ������� txbyte ������ ��� �������� � �������� ����� �� 1
	cli();
	_ctx->txbyte = _txbyte;
	_ctx->txbitcount = 1 + PUSART_BITS + 1;							// ������ ������� ��� ������ 1 ��������� ���, ���������� ��� ������ � 1 ���� ���
	sei();
}
// Waits output flush
void pusart_flush() {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	while(_ctx->txbitcount);										// ���� ���� ���������� �������� �������� �����
}
#endif

#if (defined PUSART_RECEIVER && (defined PUSART_TIMER_COUNTER || defined PUSART_IDLE_COUNTER)) || defined PUSART_TRANSMITTER
ISR(TIM0_COMPA_vect) {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	#ifdef PUSART_TRANSMITTER
	PUSART_TXPORT = (PUSART_TXPORT & ~(1 << PUSART_TXPIN)) | ((_ctx->txbyte & 1) << PUSART_TXPIN);		// ���������� � ��� TXD ������� ��� txbyte
	_ctx->txbyte = (_ctx->txbyte >> 1) | 0x8000;														// ������� txbyte ������ �� 1 � ����� 1 � ������� ������ (0x8000)
	if(_ctx->txbitcount > 0) _ctx->txbitcount--;														// ���� ���� �������� (������ ��� ������ ����), �� ��������� ��� �� �������.
	#endif

	#if defined PUSART_8BITCOUNTERS && (defined PUSART_TIMER_COUNTER || defined PUSART_IDLE_COUNTER)
	_ctx->divtimer++;
	if(_ctx->divtimer >= (1 << PUSART_8BITCOUNTERS)) {
	_ctx->divtimer = 0;
	#endif
	#ifdef PUSART_TIMER_COUNTER
	_ctx->timer++;
	#endif
	#ifdef PUSART_IDLE_COUNTER
	_ctx->idle_counter++;
	#endif
	#if defined PUSART_8BITCOUNTERS && (defined PUSART_TIMER_COUNTER || defined PUSART_IDLE_COUNTER)
	}
	#endif
}
#endif


/// ********************************************************************
/// PUSART Init
/// ********************************************************************

#if defined PUSART_RECEIVER || defined PUSART_TRANSMITTER
void pusart_init() {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	#ifdef PUSART_RECEIVER
	PUSART_RXDDR &= ~(1 << PUSART_RXPIN);	// ������ ����������� ����� �� ����� ��� ����
	PUSART_RXPORT |= (1 << PUSART_RXPIN);	// ����������� � ������� ���� RX
	MCUCR = (2 << ISC00);					// ������ ���������� INT0 �� ������� ������ ��������
	GIMSK = (1 << INT0);					// ��������� ���������� INT0
	#endif
	#ifdef PUSART_TRANSMITTER
	_ctx->txbyte = 0xFFFF;
	PUSART_TXDDR |= (1 << PUSART_TXPIN);	// ������ ����������� ����� �� �������� ��� �����
	PUSART_TXPORT |= (1 << PUSART_TXPIN);	// ����� ������� � ����� TX
	OCR0A = PUSART_TIMBAUD;					// ������ �������� �������� OCR0A � ������������ � ���������
	TIMSK0 = (1 << OCIE0A);					// ��������� ���������� TIM0_COMPA
	TCCR0B = (PUSART_TIMDIV << CS00);		// ������ �������� ����� ������� � ������������ � ���������
	#endif
	TCCR0A = (1 << WGM01);					// ����� ������� CTC (������� TCNT0 �� ���������� OCR0A)
}
#endif
