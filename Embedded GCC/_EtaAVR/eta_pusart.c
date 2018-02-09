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
	if(PUSART_RXPORT & (1 << PUSART_RXPIN)) _ctx->rxvalue |= (1 << (PUSART_BITS - 1));	// Проверяем в каком состоянии вход RX, eсли в 1, то пишем 1 в старший разряд rxvalue
	if(!(--_ctx->rxbitcount)) {															// Уменьшаем на 1 счетчик бит и проверяем не стал ли он нулем
		TIMSK0 &= ~(1 << OCIE0B);														// Если да, запрещаем прерывание TIM0_COMPB
		TIFR0 = (1 << OCF0B);															// Очищаем флаг прерывания TIM0_COMPB
		GIFR = (1 << INTF0);															// Очищаем флаг прерывания по INT0
		GIMSK |= (1 << INT0);															// Разрешаем прерывание INT0
		_ctx->rxvalue_ready = _ctx->rxvalue | 0x8000;									// Записать полученое значение как готовое в rxvalue_ready
	}
	else { _ctx->rxvalue >>= 1; }														// Иначе сдвигаем rxvalue вправо на 1
}
ISR(INT0_vect) {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	MCUCR |= (2 << ISC00); GIFR = (1 << INTF0);
	_ctx->rxbitcount = 1 + PUSART_BITS;													// 1 стартовый бит и количество бит данных
	_ctx->rxvalue = 0;																	// Обнуляем содержимое rxvalue
	uint8_t _cnt = TCNT0;
	if(_cnt < (PUSART_TIMBAUD >> 1)) { OCR0B = _cnt + (PUSART_TIMBAUD >> 1); }			// Если таймер не досчитал до середины текущего периода, то прерывание произойдет в текущем периоде спустя пол периода
	else { OCR0B = _cnt - (PUSART_TIMBAUD >> 1); }										// Иначе прерывание произойдет уже в следующем периоде таймера
	GIMSK &= ~(1 << INT0);																// Запрещаем прерывание по INT0
	TIFR0 = (1 << OCF0B);												// Очищаем флаги прерываний TIM0_COMPA (B)
	TIMSK0 |= (1 << OCIE0B);															// Разрешаем прерывание по OCR0B
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
	while(_ctx->txbitcount);										// Ждем пока закончится передача текущего байта
	uint16_t _txbyte = (value | ~((1 << PUSART_BITS) - 1)) << 1;	// Пишем в младшие разряды txbyte данные для передачи и сдвигаем влево на 1
	cli();
	_ctx->txbyte = _txbyte;
	_ctx->txbitcount = 1 + PUSART_BITS + 1;							// Задаем счетчик бит равным 1 стартовый бит, количество бит данных и 1 стоп бит
	sei();
}
// Waits output flush
void pusart_flush() {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	while(_ctx->txbitcount);										// Ждем пока закончится передача текущего байта
}
#endif

#if (defined PUSART_RECEIVER && (defined PUSART_TIMER_COUNTER || defined PUSART_IDLE_COUNTER)) || defined PUSART_TRANSMITTER
ISR(TIM0_COMPA_vect) {
	pusart_ctx_t volatile *_ctx = (pusart_ctx_t volatile *)&pusart_ctx; force_ptr(_ctx);
	#ifdef PUSART_TRANSMITTER
	PUSART_TXPORT = (PUSART_TXPORT & ~(1 << PUSART_TXPIN)) | ((_ctx->txbyte & 1) << PUSART_TXPIN);		// Выставляем в бит TXD младший бит txbyte
	_ctx->txbyte = (_ctx->txbyte >> 1) | 0x8000;														// Двигаем txbyte вправо на 1 и пишем 1 в старший разряд (0x8000)
	if(_ctx->txbitcount > 0) _ctx->txbitcount--;														// Если идет передача (счетик бит больше нуля), то уменьшаем его на единицу.
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
	PUSART_RXDDR &= ~(1 << PUSART_RXPIN);	// Задаем направление порта на прием как вход
	PUSART_RXPORT |= (1 << PUSART_RXPIN);	// Подтягиваем к единице вход RX
	MCUCR = (2 << ISC00);					// Задаем прерывание INT0 по заднему фронту импульса
	GIMSK = (1 << INT0);					// Разрешаем прерывание INT0
	#endif
	#ifdef PUSART_TRANSMITTER
	_ctx->txbyte = 0xFFFF;
	PUSART_TXDDR |= (1 << PUSART_TXPIN);	// Задаем направление порта на передачу как выход
	PUSART_TXPORT |= (1 << PUSART_TXPIN);	// Пишем единицу в выход TX
	OCR0A = PUSART_TIMBAUD;					// Задаем значение регистра OCR0A в соответствии с бодрейтом
	TIMSK0 = (1 << OCIE0A);					// Разрешаем прерывание TIM0_COMPA
	TCCR0B = (PUSART_TIMDIV << CS00);		// Задаем скорость счета таймера в соответствии с делителем
	#endif
	TCCR0A = (1 << WGM01);					// Режим таймера CTC (очистка TCNT0 по достижению OCR0A)
}
#endif
