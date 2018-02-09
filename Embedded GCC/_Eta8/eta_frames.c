/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_frames.c
 ***********************************************************************/

#include <avr/interrupt.h>
#include "eta_frames.h"
#include "eta_crc.h"

#if defined FRAMES_8B || defined FRAMES_9B

/// ********************************************************************
/// USART Frame Receiver Capability
/// ********************************************************************

#ifdef FRAMES_RECEIVER
// This flag is set on USART Receiver error
bit frames_rx_buffer_error;
// This flag is set on USART Receiver buffer overflow
bit frames_rx_buffer_overflow;
// This flag is set on USART Receiver frame collision
bit frames_rx_frame_collision;

#define FRAMES_RX_BUFFER_SIZE (1 << FRAMES_RX_BUFFER_SIZESHIFT)
#define FRAMES_RX_BUFFER_SIZEMASK (FRAMES_RX_BUFFER_SIZE - 1)

typedef enum {
	FRAMES_RECEIVERSTEP_STOP,
	FRAMES_RECEIVERSTEP_ADDRESS,
	FRAMES_RECEIVERSTEP_COMMAND,
	FRAMES_RECEIVERSTEP_SIZE,
	FRAMES_RECEIVERSTEP_DATA,
	FRAMES_RECEIVERSTEP_CRC,
} eFrames_ReceiverStep;

typedef struct {
	eFrames_ReceiverStep step;
	uint8_t crc;
	#ifdef FRAMES_8B
	uint8_t rotation_byte;
	uint8_t rotation_index;
	#endif
	uint8_t frame_address;
	uint8_t frame_address_ready;
	uint8_t frame_command;
	uint8_t frame_command_ready;
	uint8_t frame_size;
	uint8_t frame_size_ready;
	uint8_t frame_size_left;
	#if FRAMES_RX_BUFFER_SIZESHIFT <= 8
	uint8_t wr_index;
	uint8_t rd_index;
	uint8_t frame_size_index;
	uint8_t volatile frame_count;
	#else
	uint16_t wr_index;
	uint16_t rd_index;
	uint16_t frame_size_index;
	uint16_t volatile frame_count;
	#endif
	#if FRAMES_RX_BUFFER_SIZESHIFT <= 7
	uint8_t volatile counter;
	#else
	uint16_t volatile counter;
	#endif
} sFrames_RxContext;

static sFrames_RxContext frames_rx_context;
static uint8_t frames_rx_buffer[FRAMES_RX_BUFFER_SIZE];

static void _frames_discard_current_frame() {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);

	__typeof__(_rx_ctx->frame_size_index) _index_tmp = _rx_ctx->frame_size_index;
	_rx_ctx->counter -= (_rx_ctx->wr_index - _index_tmp);
	_rx_ctx->wr_index = _index_tmp;
	_rx_ctx->step = FRAMES_RECEIVERSTEP_STOP;
}
static void _frames_process_receiver_byte(uint8_t byte) {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);

	// command byte
	if(_rx_ctx->step == FRAMES_RECEIVERSTEP_ADDRESS) {
		_rx_ctx->frame_address = byte;
		_rx_ctx->step++;
	}
	// command byte
	else if(_rx_ctx->step == FRAMES_RECEIVERSTEP_COMMAND) {
		_rx_ctx->frame_command = byte;
		_rx_ctx->step++;
	}
	// size of the frame
	else if(_rx_ctx->step == FRAMES_RECEIVERSTEP_SIZE) {
		_rx_ctx->frame_size_left = _rx_ctx->frame_size = byte;
		_rx_ctx->step = byte ? FRAMES_RECEIVERSTEP_DATA : FRAMES_RECEIVERSTEP_CRC;
	}
	// useful frame data
	else if(_rx_ctx->step == FRAMES_RECEIVERSTEP_DATA) {
		if(_rx_ctx->counter < FRAMES_RX_BUFFER_SIZE) {
			_rx_ctx->counter++; _rx_ctx->frame_size_left--; frames_rx_buffer[_rx_ctx->wr_index++] = byte;
			if(!_rx_ctx->frame_size_left) _rx_ctx->step++;
		}
		else { frames_rx_buffer_overflow = true; _frames_discard_current_frame(); }
	}
	// expecting CRC
	else if (_rx_ctx->step == FRAMES_RECEIVERSTEP_CRC) {
		if(byte == _rx_ctx->crc) {
			// The whole frame has arrived successfully
			if(!_rx_ctx->frame_count) { _rx_ctx->frame_address_ready = _rx_ctx->frame_address; _rx_ctx->frame_command_ready = _rx_ctx->frame_command; _rx_ctx->frame_size_ready = _rx_ctx->frame_size; }
			else {
				__typeof__(_rx_ctx->frame_size_index) _index_tmp = _rx_ctx->frame_size_index;
				frames_rx_buffer[_index_tmp++] = _rx_ctx->frame_address;
				#if FRAMES_RX_BUFFER_SIZESHIFT != 8
				_index_tmp &= FRAMES_RX_BUFFER_SIZEMASK;
				#endif
				frames_rx_buffer[_index_tmp++] = _rx_ctx->frame_command;
				#if FRAMES_RX_BUFFER_SIZESHIFT != 8
				_index_tmp &= FRAMES_RX_BUFFER_SIZEMASK;
				#endif
				frames_rx_buffer[_index_tmp] = _rx_ctx->frame_size;
			}
			_rx_ctx->frame_count++;
			_rx_ctx->step = FRAMES_RECEIVERSTEP_STOP;
		}
		else { frames_rx_frame_collision = true; _frames_discard_current_frame(); }
	}

	#if FRAMES_RX_BUFFER_SIZESHIFT != 8
	_rx_ctx->wr_index &= FRAMES_RX_BUFFER_SIZEMASK;
	#endif
	_rx_ctx->crc = crc8_update(_rx_ctx->crc, byte);
}

// Process incomming data to the USART Receiver buffer from interrupt service routine
#if defined FRAMES_8B
void frames8b_push_data_from_isr(uint8_t data) {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);

	// Got synchronization data
	if(data & 0x80) {
		if(_rx_ctx->step != FRAMES_RECEIVERSTEP_STOP) { frames_rx_frame_collision = true; _frames_discard_current_frame(); }
		if(_rx_ctx->counter <= (FRAMES_RX_BUFFER_SIZE - 3)) {
			_rx_ctx->counter += 3; _rx_ctx->frame_size_index = _rx_ctx->wr_index; _rx_ctx->wr_index += 3;
			_rx_ctx->step++; _rx_ctx->crc = 0; _rx_ctx->rotation_index = 7; _rx_ctx->rotation_byte = data & 0x7F;
		}
		else { frames_rx_buffer_overflow = true; }
	}
	else if(_rx_ctx->step != FRAMES_RECEIVERSTEP_STOP) {
		uint8_t _rotation_index = _rx_ctx->rotation_index, _rotation_byte = _rx_ctx->rotation_byte;
		uint16_t _data16 = data << _rotation_index;
		_rotation_byte |= (uint8_t)_data16; _rotation_index += 7;
		if(_rotation_index >= 8) {
			_frames_process_receiver_byte(_rotation_byte);
			_rotation_byte = _data16 >> 8; _rotation_index -= 8;
		}
		_rx_ctx->rotation_index = _rotation_index; _rx_ctx->rotation_byte = _rotation_byte;
	}
}
#elif defined FRAMES_9B
void frames9b_push_data_from_isr(uint16_t data) {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);

	// Got synchronization data
	if(data & 0x100) {
		if(_rx_ctx->step != FRAMES_RECEIVERSTEP_STOP) { frames_rx_frame_collision = true; _frames_discard_current_frame(); }
		if(_rx_ctx->counter <= (FRAMES_RX_BUFFER_SIZE - 3)) {
			_rx_ctx->counter += 3; _rx_ctx->frame_size_index = _rx_ctx->wr_index; _rx_ctx->wr_index += 3;
			_rx_ctx->step++; _rx_ctx->crc = 0;
		}
		else { frames_rx_buffer_overflow = true; }
	}
	_frames_process_receiver_byte((uint8_t)data);
}
#endif

// Get address of the first available frame in the USART Receiver buffer
uint8_t frames_get_frame_address() {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);
	return _rx_ctx->frame_count ? _rx_ctx->frame_address_ready : 0;
}

// Get command of the first available frame in the USART Receiver buffer
uint8_t frames_get_frame_command() {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);
	return _rx_ctx->frame_count ? _rx_ctx->frame_command_ready : 0;
}

// Get size of the first available frame in the USART Receiver buffer
uint8_t frames_get_frame_size() {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);
	return _rx_ctx->frame_count ? _rx_ctx->frame_size_ready : 0;
}

// Get data of the first available frame in the USART Receiver buffer
uint8_t frames_get_frame_data(void *dst, uint8_t offset, uint8_t size) {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);

	if(!_rx_ctx->frame_count) return 0;
	uint8_t _ret_size = 0;
	minwith(offset, _rx_ctx->frame_size_ready); minwith(size, _rx_ctx->frame_size_ready - offset); _ret_size = size;
	__typeof__(_rx_ctx->rd_index) _src_index = _rx_ctx->rd_index + offset + 2;
	uint8_t *_dst = (uint8_t *)dst;
	while(size--) {
		#if FRAMES_RX_BUFFER_SIZESHIFT != 8
		_src_index &= FRAMES_RX_BUFFER_SIZEMASK;
		#endif
		*_dst++ = frames_rx_buffer[_src_index++];
	}
	return _ret_size;
}

// Remove the first available frame from the USART Receiver buffer
void frames_remove_frame() {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);

	if(!_rx_ctx->frame_count) return;
	_rx_ctx->rd_index += (_rx_ctx->frame_size_ready + 3);
	#if FRAMES_RX_BUFFER_SIZESHIFT != 8
	_rx_ctx->rd_index &= FRAMES_RX_BUFFER_SIZEMASK;
	#endif
	cli();
	_rx_ctx->frame_count--;
	_rx_ctx->counter -= (_rx_ctx->frame_size_ready + 3);
	if(_rx_ctx->frame_count) {
		__typeof__(_rx_ctx->rd_index) _rx_rd_index_tmp = _rx_ctx->rd_index;
		_rx_ctx->frame_address_ready = frames_rx_buffer[_rx_rd_index_tmp++];
		#if FRAMES_RX_BUFFER_SIZESHIFT != 8
		_rx_rd_index_tmp &= FRAMES_RX_BUFFER_SIZEMASK;
		#endif
		_rx_ctx->frame_command_ready = frames_rx_buffer[_rx_rd_index_tmp++];
		#if FRAMES_RX_BUFFER_SIZESHIFT != 8
		_rx_rd_index_tmp &= FRAMES_RX_BUFFER_SIZEMASK;
		#endif
		_rx_ctx->frame_size_ready = frames_rx_buffer[_rx_rd_index_tmp];
	}
	sei();
}

// Gets a value indicating whether there is a ready frame in the USART Receiver buffer
bool frames_is_frame_available() {
	sFrames_RxContext *_rx_ctx = &frames_rx_context; force_ptr(_rx_ctx);
	return _rx_ctx->frame_count;
}
#endif /* FRAMES_RECEIVER */

/// ********************************************************************
/// USART Frame Transmitter Capability
/// ********************************************************************

#ifdef FRAMES_TRANSMITTER
#define FRAMES_TX_BUFFER_SIZE (1 << FRAMES_TX_BUFFER_SIZESHIFT)
#define FRAMES_TX_BUFFER_SIZEMASK (FRAMES_TX_BUFFER_SIZE - 1)

typedef enum {
	FRAMES_TRANSMITTER_STOP,
	FRAMES_TRANSMITTER_ADDRESS,
	FRAMES_TRANSMITTER_COMMAND,
	FRAMES_TRANSMITTER_SIZE,
	FRAMES_TRANSMITTER_DATA,
	FRAMES_TRANSMITTER_CRC,
} eFrames_TransmitterStep;

typedef enum {
	FRAMES_TXBYTEFLAG_VALID_BYTE = (1 << 0),
	FRAMES_TXBYTEFLAG_SYNCHRONIZE = (1 << 1),
	FRAMES_TXBYTEFLAG_NEED_FLUSH = (1 << 2),
} eFrames_TxByteFlags;

typedef struct {
	eFrames_TransmitterStep step;
	uint8_t crc;
	#ifdef FRAMES_8B
	uint8_t rotation_byte;
	uint8_t rotation_index;
	#endif
	uint8_t frame_size;
	uint8_t frame_size_left;
	#if FRAMES_TX_BUFFER_SIZESHIFT <= 8
	uint8_t wr_index;
	uint8_t rd_index;
	uint8_t volatile frame_count;
	#else
	uint16_t wr_index;
	uint16_t rd_index;
	uint16_t volatile frame_count;
	#endif
	#if FRAMES_TX_BUFFER_SIZESHIFT <= 7
	uint8_t volatile counter;
	#else
	uint16_t volatile counter;
	#endif
} sFrames_TxContext;

static sFrames_TxContext frames_tx_context;
static uint8_t frames_tx_buffer[FRAMES_TX_BUFFER_SIZE];

static eFrames_TxByteFlags _frames_pick_transmitter_byte(uint8_t *data) {
	sFrames_TxContext *_tx_ctx = &frames_tx_context; force_ptr(_tx_ctx);

	uint8_t _byte; eFrames_TxByteFlags _flags = 0; bool _fifo_act = false;
	// try to find new frame
	if(_tx_ctx->step == FRAMES_TRANSMITTER_STOP) {
		if(_tx_ctx->frame_count) { _tx_ctx->step++; _tx_ctx->crc = 0; }
	}

	// command byte
	if(_tx_ctx->step == FRAMES_TRANSMITTER_ADDRESS) {
		_byte = frames_tx_buffer[_tx_ctx->rd_index]; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE | FRAMES_TXBYTEFLAG_SYNCHRONIZE; _fifo_act = true;
		_tx_ctx->step++;
	}
	// command byte
	if(_tx_ctx->step == FRAMES_TRANSMITTER_COMMAND) {
		_byte = frames_tx_buffer[_tx_ctx->rd_index]; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE; _fifo_act = true;
		_tx_ctx->step++;
	}
	// size of the frame
	else if(_tx_ctx->step == FRAMES_TRANSMITTER_SIZE) {
		_byte = _tx_ctx->frame_size = _tx_ctx->frame_size_left = frames_tx_buffer[_tx_ctx->rd_index]; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE; _fifo_act = true;
		_tx_ctx->step = _byte ? FRAMES_TRANSMITTER_DATA : FRAMES_TRANSMITTER_CRC;
	}
	// useful frame data
	else if(_tx_ctx->step == FRAMES_TRANSMITTER_DATA) {
		_byte = frames_tx_buffer[_tx_ctx->rd_index]; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE; _fifo_act = true;
		_tx_ctx->frame_size_left--;
		if(!_tx_ctx->frame_size_left) _tx_ctx->step++;
	}
	// CRC
	else if(_tx_ctx->step == FRAMES_TRANSMITTER_CRC) {
		_byte = _tx_ctx->crc; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE | FRAMES_TXBYTEFLAG_NEED_FLUSH;
		_tx_ctx->frame_count--;
		_tx_ctx->step = FRAMES_TRANSMITTER_STOP;
	}

	if(_fifo_act) {
		_tx_ctx->rd_index = (_tx_ctx->rd_index + 1);
		#if FRAMES_TX_BUFFER_SIZESHIFT != 8
		_tx_ctx->rd_index &= FRAMES_TX_BUFFER_SIZEMASK;
		#endif
		_tx_ctx->counter--;
	}
	if(_flags) { *data = _byte; _tx_ctx->crc = crc8_update(_tx_ctx->crc, _byte); }
	return _flags;
}

// Process outgoing data from the USART Transmitter buffer from interrupt service routine
#if defined FRAMES_8B
bool frames8b_pop_data_from_isr() {
	sFrames_TxContext *_tx_ctx = &frames_tx_context; force_ptr(_tx_ctx);

	bool _ret = false; uint8_t _rotation_index = _tx_ctx->rotation_index; uint16_t _rotation_byte = _tx_ctx->rotation_byte;
	if(_rotation_index >= 7) { _ret = true; USARTINIT_WRITE_DATA(_rotation_byte); _rotation_index = _rotation_byte = 0; }
	else {
		uint8_t _data8; eFrames_TxByteFlags _flags = _frames_pick_transmitter_byte(&_data8);
		if(_flags) {
			_ret = true;
			_rotation_byte |= (uint16_t)_data8 << _rotation_index;
			USARTINIT_WRITE_DATA((_rotation_byte & 0x7F) | ((_flags & FRAMES_TXBYTEFLAG_SYNCHRONIZE) ? 0x80 : 0)); _rotation_index = (_flags & FRAMES_TXBYTEFLAG_NEED_FLUSH) ? 7 : _rotation_index + 1; _rotation_byte >>= 7;
		}
	}
	_tx_ctx->rotation_index = _rotation_index; _tx_ctx->rotation_byte = _rotation_byte;
	return _ret;
}
#elif defined FRAMES_9B
bool frames9b_pop_data_from_isr() {
	uint8_t _data8; eFrames_TxByteFlags _flags = _frames_pick_transmitter_byte(portparam &_data8);
	if(_flags) { USARTINIT_WRITE_DATA((_flags & FRAMES_TXBYTEFLAG_SYNCHRONIZE) ? (_data8 | 0x100) : _data8); }
	return _flags;
}
#endif

// Write a frame to the USART Transmitter buffer
bool frames_put_frame(uint8_t addr, uint8_t cmd, void const *data, uint8_t size) {
	sFrames_TxContext *_tx_ctx = &frames_tx_context; force_ptr(_tx_ctx);

	uint16_t _total_size = size + 3;
	if(FRAMES_TX_BUFFER_SIZE < _total_size) return false;
	while((FRAMES_TX_BUFFER_SIZE - (uint16_t)_tx_ctx->counter) < _total_size) FRAMES_YIELD();
	frames_tx_buffer[_tx_ctx->wr_index++] = addr;
	#if FRAMES_TX_BUFFER_SIZESHIFT != 8
	_tx_ctx->wr_index &= FRAMES_TX_BUFFER_SIZEMASK;
	#endif
	frames_tx_buffer[_tx_ctx->wr_index++] = cmd;
	#if FRAMES_TX_BUFFER_SIZESHIFT != 8
	_tx_ctx->wr_index &= FRAMES_TX_BUFFER_SIZEMASK;
	#endif
	frames_tx_buffer[_tx_ctx->wr_index++] = size;
	#if FRAMES_TX_BUFFER_SIZESHIFT != 8
	_tx_ctx->wr_index &= FRAMES_TX_BUFFER_SIZEMASK;
	#endif
	uint8_t const *_data_ptr = (uint8_t const *)data; uint8_t _size_cpy = size;
	while(_size_cpy--) {
		frames_tx_buffer[_tx_ctx->wr_index++] = *_data_ptr++;
		#if FRAMES_TX_BUFFER_SIZESHIFT != 8
		_tx_ctx->wr_index &= FRAMES_TX_BUFFER_SIZEMASK;
		#endif
	}
	cli();
	_tx_ctx->counter += _total_size; _tx_ctx->frame_count++;
	USARTINIT_ENABLE_TX_INTERRUPT();
	sei();
	return true;
}

// Write a frame to the USART Transmitter buffer (extended version)
bool frames_put_frame_ex(uint8_t addr, uint8_t cmd, uint8_t datas_cnt, ...) { va_list _ap; va_start(_ap, datas_cnt); bool _res = frames_put_frame_exva(addr, cmd, datas_cnt, _ap); va_end(_ap); return _res; }
bool frames_put_frame_exva(uint8_t addr, uint8_t cmd, uint8_t datas_cnt, va_list ap) {
	sFrames_TxContext *_tx_ctx = &frames_tx_context; force_ptr(_tx_ctx);

	uint16_t _total_size = 3;
	va_list _ap_copy; va_copy(_ap_copy, ap);
	uint8_t _datas_cnt_cpy = datas_cnt;
	while(_datas_cnt_cpy--) { void const *_data = va_arg(_ap_copy, void const *); (void)_data; _total_size += va_arg(_ap_copy, uint16_t); }
	if(FRAMES_TX_BUFFER_SIZE < _total_size) return false;
	while((FRAMES_TX_BUFFER_SIZE - (uint16_t)_tx_ctx->counter) < _total_size) FRAMES_YIELD();
	frames_tx_buffer[_tx_ctx->wr_index++] = addr;
	#if FRAMES_TX_BUFFER_SIZESHIFT != 8
	_tx_ctx->wr_index &= FRAMES_TX_BUFFER_SIZEMASK;
	#endif
	frames_tx_buffer[_tx_ctx->wr_index++] = cmd;
	#if FRAMES_TX_BUFFER_SIZESHIFT != 8
	_tx_ctx->wr_index &= FRAMES_TX_BUFFER_SIZEMASK;
	#endif
	frames_tx_buffer[_tx_ctx->wr_index++] = _total_size - 3;
	#if FRAMES_TX_BUFFER_SIZESHIFT != 8
	_tx_ctx->wr_index &= FRAMES_TX_BUFFER_SIZEMASK;
	#endif
	while(datas_cnt--) {
		uint8_t const *_data_ptr = (uint8_t const *)va_arg(ap, uint8_t const *); uint8_t _data_len = va_arg(ap, uint16_t);
		while(_data_len--) {
			frames_tx_buffer[_tx_ctx->wr_index++] = *_data_ptr++;
			#if FRAMES_TX_BUFFER_SIZESHIFT != 8
			_tx_ctx->wr_index &= FRAMES_TX_BUFFER_SIZEMASK;
			#endif
		}
	}
	cli();
	_tx_ctx->counter += _total_size; _tx_ctx->frame_count++;
	USARTINIT_ENABLE_TX_INTERRUPT();
	sei();
	return true;
}

// Waits until all date gone from Transmitter buffer
void frames_flush() { while(frames_tx_context.frame_count) FRAMES_YIELD(); }

// Gets a value indicating whether USART Transmitter is free
bool frames_is_transmitter_free() { return !frames_tx_context.frame_count; }

#endif /* FRAMES_TRANSMITTER */

#endif /* defined FRAMES_8B || defined FRAMES_9B */
