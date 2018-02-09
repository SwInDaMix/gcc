/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_frames.c
 ***********************************************************************/

#include "stdperiph.h"
#include "eta_frames.h"
#include "eta_crc.h"

#if defined FRAMES_8B || defined FRAMES_9B

// === PORT 0 ===
#if defined FRAMES0_PERIPH && defined FRAMES0_IRQ && (FRAMES0_RX_BUFFER_SIZESHIFT > 0 || FRAMES0_TX_BUFFER_SIZESHIFT > 0)
#if FRAMES0_RX_BUFFER_SIZESHIFT > 0
#define FRAMES0_RX_BUFFER_SIZE (1 << FRAMES0_RX_BUFFER_SIZESHIFT)
static uint8_t Frames0_RxBuffer[FRAMES0_RX_BUFFER_SIZE];
#else
#define FRAMES0_RX_BUFFER_SIZE 0
#define Frames0_RxBuffer NULL
#endif
#if FRAMES0_TX_BUFFER_SIZESHIFT > 0
#define FRAMES0_TX_BUFFER_SIZE (1 << FRAMES0_TX_BUFFER_SIZESHIFT)
static uint8_t Frames0_TxBuffer[FRAMES0_TX_BUFFER_SIZE];
#else
#define FRAMES0_TX_BUFFER_SIZE 0
#define Frames0_TxBuffer NULL
#endif
#define FRAMES_PERIPHS FRAMES0_PERIPH
#define FRAMES_IRQS FRAMES0_IRQ
#define FRAMES_RX_SIZES FRAMES0_RX_BUFFER_SIZE
#define FRAMES_TX_SIZES FRAMES0_TX_BUFFER_SIZE
#define FRAMES_RX_BUFFERS Frames0_RxBuffer
#define FRAMES_TX_BUFFERS Frames0_TxBuffer

// === PORT 1 ===
#if defined FRAMES1_PERIPH && defined FRAMES1_IRQ && (FRAMES1_RX_BUFFER_SIZESHIFT > 0 || FRAMES1_TX_BUFFER_SIZESHIFT > 0)
#if FRAMES1_RX_BUFFER_SIZESHIFT > 0
#define FRAMES1_RX_BUFFER_SIZE (1 << FRAMES1_RX_BUFFER_SIZESHIFT)
static uint8_t Frames1_RxBuffer[FRAMES1_RX_BUFFER_SIZE];
#else
#define FRAMES1_RX_BUFFER_SIZE 0
#define Frames1_RxBuffer NULL
#endif
#if FRAMES1_TX_BUFFER_SIZESHIFT > 0
#define FRAMES1_TX_BUFFER_SIZE (1 << FRAMES1_TX_BUFFER_SIZESHIFT)
static uint8_t Frames1_TxBuffer[FRAMES1_TX_BUFFER_SIZE];
#else
#define FRAMES1_TX_BUFFER_SIZE 0
#define Frames1_TxBuffer NULL
#endif
#undef FRAMES_PERIPHS
#undef FRAMES_IRQS
#undef FRAMES_RX_SIZES
#undef FRAMES_TX_SIZES
#undef FRAMES_RX_BUFFERS
#undef FRAMES_TX_BUFFERS
#define FRAMES_PERIPHS FRAMES0_PERIPH, FRAMES1_PERIPH
#define FRAMES_IRQS FRAMES0_IRQ, FRAMES1_IRQ
#define FRAMES_RX_SIZES FRAMES0_RX_BUFFER_SIZE, FRAMES1_RX_BUFFER_SIZE
#define FRAMES_TX_SIZES FRAMES0_TX_BUFFER_SIZE, FRAMES1_TX_BUFFER_SIZE
#define FRAMES_RX_BUFFERS Frames0_RxBuffer, Frames1_RxBuffer
#define FRAMES_TX_BUFFERS Frames0_TxBuffer, Frames1_TxBuffer

// === PORT 2 ===
#if defined FRAMES2_PERIPH && defined FRAMES2_IRQ && (FRAMES2_RX_BUFFER_SIZESHIFT > 0 || FRAMES2_TX_BUFFER_SIZESHIFT > 0)
#if FRAMES2_RX_BUFFER_SIZESHIFT > 0
#define FRAMES2_RX_BUFFER_SIZE (1 << FRAMES2_RX_BUFFER_SIZESHIFT)
static uint8_t Frames2_RxBuffer[FRAMES2_RX_BUFFER_SIZE];
#else
#define FRAMES2_RX_BUFFER_SIZE 0
#define Frames2_RxBuffer NULL
#endif
#if FRAMES2_TX_BUFFER_SIZESHIFT > 0
#define FRAMES2_TX_BUFFER_SIZE (1 << FRAMES2_TX_BUFFER_SIZESHIFT)
static uint8_t Frames2_TxBuffer[FRAMES2_TX_BUFFER_SIZE];
#else
#define FRAMES2_TX_BUFFER_SIZE 0
#define Frames2_TxBuffer NULL
#endif
#undef FRAMES_PERIPHS
#undef FRAMES_IRQS
#undef FRAMES_RX_SIZES
#undef FRAMES_TX_SIZES
#undef FRAMES_RX_BUFFERS
#undef FRAMES_TX_BUFFERS
#define FRAMES_PERIPHS FRAMES0_PERIPH, FRAMES1_PERIPH, FRAMES2_PERIPH
#define FRAMES_IRQS FRAMES0_IRQ, FRAMES1_IRQ, FRAMES2_IRQ
#define FRAMES_RX_SIZES FRAMES0_RX_BUFFER_SIZE, FRAMES1_RX_BUFFER_SIZE, FRAMES2_RX_BUFFER_SIZE
#define FRAMES_TX_SIZES FRAMES0_TX_BUFFER_SIZE, FRAMES1_TX_BUFFER_SIZE, FRAMES2_TX_BUFFER_SIZE
#define FRAMES_RX_BUFFERS Frames0_RxBuffer, Frames1_RxBuffer, Frames2_RxBuffer
#define FRAMES_TX_BUFFERS Frames0_TxBuffer, Frames1_TxBuffer, Frames2_TxBuffer

// === PORT 3 ===
#if defined FRAMES3_PERIPH && defined FRAMES3_IRQ && (FRAMES3_RX_BUFFER_SIZESHIFT > 0 || FRAMES3_TX_BUFFER_SIZESHIFT > 0)
#if FRAMES3_RX_BUFFER_SIZESHIFT > 0
#define FRAMES3_RX_BUFFER_SIZE (1 << FRAMES3_RX_BUFFER_SIZESHIFT)
static uint8_t Frames3_RxBuffer[FRAMES3_RX_BUFFER_SIZE];
#else
#define FRAMES3_RX_BUFFER_SIZE 0
#define Frames3_RxBuffer NULL
#endif
#if FRAMES3_TX_BUFFER_SIZESHIFT > 0
#define FRAMES3_TX_BUFFER_SIZE (1 << FRAMES3_TX_BUFFER_SIZESHIFT)
static uint8_t Frames3_TxBuffer[FRAMES3_TX_BUFFER_SIZE];
#else
#define FRAMES3_TX_BUFFER_SIZE 0
#define Frames3_TxBuffer NULL
#endif
#undef FRAMES_PERIPHS
#undef FRAMES_IRQS
#undef FRAMES_RX_SIZES
#undef FRAMES_TX_SIZES
#undef FRAMES_RX_BUFFERS
#undef FRAMES_TX_BUFFERS
#define FRAMES_PERIPHS FRAMES0_PERIPH, FRAMES1_PERIPH, FRAMES2_PERIPH, FRAMES3_PERIPH
#define FRAMES_IRQS FRAMES0_IRQ, FRAMES1_IRQ, FRAMES2_IRQ, FRAMES3_IRQ
#define FRAMES_RX_SIZES FRAMES0_RX_BUFFER_SIZE, FRAMES1_RX_BUFFER_SIZE, FRAMES2_RX_BUFFER_SIZE, FRAMES3_RX_BUFFER_SIZE
#define FRAMES_TX_SIZES FRAMES0_TX_BUFFER_SIZE, FRAMES1_TX_BUFFER_SIZE, FRAMES2_TX_BUFFER_SIZE, FRAMES3_TX_BUFFER_SIZE
#define FRAMES_RX_BUFFERS Frames0_RxBuffer, Frames1_RxBuffer, Frames2_RxBuffer, Frames3_RxBuffer
#define FRAMES_TX_BUFFERS Frames0_TxBuffer, Frames1_TxBuffer, Frames2_TxBuffer, Frames3_TxBuffer

// === PORT 4 ===
#if defined FRAMES4_PERIPH && defined FRAMES4_IRQ && (FRAMES4_RX_BUFFER_SIZESHIFT > 0 || FRAMES4_TX_BUFFER_SIZESHIFT > 0)
#if FRAMES4_RX_BUFFER_SIZESHIFT > 0
#define FRAMES4_RX_BUFFER_SIZE (1 << FRAMES4_RX_BUFFER_SIZESHIFT)
static uint8_t Frames4_RxBuffer[FRAMES4_RX_BUFFER_SIZE];
#else
#define FRAMES4_RX_BUFFER_SIZE 0
#define Frames4_RxBuffer NULL
#endif
#if FRAMES4_TX_BUFFER_SIZESHIFT > 0
#define FRAMES4_TX_BUFFER_SIZE (1 << FRAMES4_TX_BUFFER_SIZESHIFT)
static uint8_t Frames4_TxBuffer[FRAMES4_TX_BUFFER_SIZE];
#else
#define FRAMES4_TX_BUFFER_SIZE 0
#define Frames4_TxBuffer NULL
#endif
#undef FRAMES_PERIPHS
#undef FRAMES_IRQS
#undef FRAMES_RX_SIZES
#undef FRAMES_TX_SIZES
#undef FRAMES_RX_BUFFERS
#undef FRAMES_TX_BUFFERS
#define FRAMES_PERIPHS FRAMES0_PERIPH, FRAMES1_PERIPH, FRAMES2_PERIPH, FRAMES3_PERIPH, FRAMES4_PERIPH
#define FRAMES_IRQS FRAMES0_IRQ, FRAMES1_IRQ, FRAMES2_IRQ, FRAMES3_IRQ, FRAMES4_IRQ
#define FRAMES_RX_SIZES FRAMES0_RX_BUFFER_SIZE, FRAMES1_RX_BUFFER_SIZE, FRAMES2_RX_BUFFER_SIZE, FRAMES3_RX_BUFFER_SIZE, FRAMES4_RX_BUFFER_SIZE
#define FRAMES_TX_SIZES FRAMES0_TX_BUFFER_SIZE, FRAMES1_TX_BUFFER_SIZE, FRAMES2_TX_BUFFER_SIZE, FRAMES3_TX_BUFFER_SIZE, FRAMES4_TX_BUFFER_SIZE
#define FRAMES_RX_BUFFERS Frames0_RxBuffer, Frames1_RxBuffer, Frames2_RxBuffer, Frames3_RxBuffer, Frames4_RxBuffer
#define FRAMES_TX_BUFFERS Frames0_TxBuffer, Frames1_TxBuffer, Frames2_TxBuffer, Frames3_TxBuffer, Frames4_TxBuffer

// === PORT 5 ===
#if defined FRAMES5_PERIPH && defined FRAMES5_IRQ && (FRAMES5_RX_BUFFER_SIZESHIFT > 0 || FRAMES5_TX_BUFFER_SIZESHIFT > 0)
#if FRAMES5_RX_BUFFER_SIZESHIFT > 0
#define FRAMES5_RX_BUFFER_SIZE (1 << FRAMES5_RX_BUFFER_SIZESHIFT)
static uint8_t Frames5_RxBuffer[FRAMES5_RX_BUFFER_SIZE];
#else
#define FRAMES5_RX_BUFFER_SIZE 0
#define Frames5_RxBuffer NULL
#endif
#if FRAMES5_TX_BUFFER_SIZESHIFT > 0
#define FRAMES5_TX_BUFFER_SIZE (1 << FRAMES5_TX_BUFFER_SIZESHIFT)
static uint8_t Frames5_TxBuffer[FRAMES5_TX_BUFFER_SIZE];
#else
#define FRAMES5_TX_BUFFER_SIZE 0
#define Frames5_TxBuffer NULL
#endif
#undef FRAMES_PERIPHS
#undef FRAMES_IRQS
#undef FRAMES_RX_SIZES
#undef FRAMES_TX_SIZES
#undef FRAMES_RX_BUFFERS
#undef FRAMES_TX_BUFFERS
#define FRAMES_PERIPHS FRAMES0_PERIPH, FRAMES1_PERIPH, FRAMES2_PERIPH, FRAMES3_PERIPH, FRAMES4_PERIPH, FRAMES5_PERIPH
#define FRAMES_IRQS FRAMES0_IRQ, FRAMES1_IRQ, FRAMES2_IRQ, FRAMES3_IRQ, FRAMES4_IRQ, FRAMES5_IRQ
#define FRAMES_RX_SIZES FRAMES0_RX_BUFFER_SIZE, FRAMES1_RX_BUFFER_SIZE, FRAMES2_RX_BUFFER_SIZE, FRAMES3_RX_BUFFER_SIZE, FRAMES4_RX_BUFFER_SIZE, FRAMES5_RX_BUFFER_SIZE
#define FRAMES_TX_SIZES FRAMES0_TX_BUFFER_SIZE, FRAMES1_TX_BUFFER_SIZE, FRAMES2_TX_BUFFER_SIZE, FRAMES3_TX_BUFFER_SIZE, FRAMES4_TX_BUFFER_SIZE, FRAMES5_TX_BUFFER_SIZE
#define FRAMES_RX_BUFFERS Frames0_RxBuffer, Frames1_RxBuffer, Frames2_RxBuffer, Frames3_RxBuffer, Frames4_RxBuffer, Frames5_RxBuffer
#define FRAMES_TX_BUFFERS Frames0_TxBuffer, Frames1_TxBuffer, Frames2_TxBuffer, Frames3_TxBuffer, Frames4_TxBuffer, Frames5_TxBuffer

// === PORT 6 ===
#if defined FRAMES6_PERIPH && defined FRAMES6_IRQ && (FRAMES6_RX_BUFFER_SIZESHIFT > 0 || FRAMES6_TX_BUFFER_SIZESHIFT > 0)
#if FRAMES6_RX_BUFFER_SIZESHIFT > 0
#define FRAMES6_RX_BUFFER_SIZE (1 << FRAMES6_RX_BUFFER_SIZESHIFT)
static uint8_t Frames6_RxBuffer[FRAMES6_RX_BUFFER_SIZE];
#else
#define FRAMES6_RX_BUFFER_SIZE 0
#define Frames6_RxBuffer NULL
#endif
#if FRAMES6_TX_BUFFER_SIZESHIFT > 0
#define FRAMES6_TX_BUFFER_SIZE (1 << FRAMES6_TX_BUFFER_SIZESHIFT)
static uint8_t Frames6_TxBuffer[FRAMES6_TX_BUFFER_SIZE];
#else
#define FRAMES6_TX_BUFFER_SIZE 0
#define Frames6_TxBuffer NULL
#endif
#undef FRAMES_PERIPHS
#undef FRAMES_IRQS
#undef FRAMES_RX_SIZES
#undef FRAMES_TX_SIZES
#undef FRAMES_RX_BUFFERS
#undef FRAMES_TX_BUFFERS
#define FRAMES_PERIPHS FRAMES0_PERIPH, FRAMES1_PERIPH, FRAMES2_PERIPH, FRAMES3_PERIPH, FRAMES4_PERIPH, FRAMES5_PERIPH, FRAMES6_PERIPH
#define FRAMES_IRQS FRAMES0_IRQ, FRAMES1_IRQ, FRAMES2_IRQ, FRAMES3_IRQ, FRAMES4_IRQ, FRAMES5_IRQ, FRAMES6_IRQ
#define FRAMES_RX_SIZES FRAMES0_RX_BUFFER_SIZE, FRAMES1_RX_BUFFER_SIZE, FRAMES2_RX_BUFFER_SIZE, FRAMES3_RX_BUFFER_SIZE, FRAMES4_RX_BUFFER_SIZE, FRAMES5_RX_BUFFER_SIZE, FRAMES6_RX_BUFFER_SIZE
#define FRAMES_TX_SIZES FRAMES0_TX_BUFFER_SIZE, FRAMES1_TX_BUFFER_SIZE, FRAMES2_TX_BUFFER_SIZE, FRAMES3_TX_BUFFER_SIZE, FRAMES4_TX_BUFFER_SIZE, FRAMES5_TX_BUFFER_SIZE, FRAMES6_TX_BUFFER_SIZE
#define FRAMES_RX_BUFFERS Frames0_RxBuffer, Frames1_RxBuffer, Frames2_RxBuffer, Frames3_RxBuffer, Frames4_RxBuffer, Frames5_RxBuffer, Frames6_RxBuffer
#define FRAMES_TX_BUFFERS Frames0_TxBuffer, Frames1_TxBuffer, Frames2_TxBuffer, Frames3_TxBuffer, Frames4_TxBuffer, Frames5_TxBuffer, Frames6_TxBuffer

// === PORT 7 ===
#if defined FRAMES7_PERIPH && defined FRAMES7_IRQ && (FRAMES7_RX_BUFFER_SIZESHIFT > 0 || FRAMES7_TX_BUFFER_SIZESHIFT > 0)
#if FRAMES7_RX_BUFFER_SIZESHIFT > 0
#define FRAMES7_RX_BUFFER_SIZE (1 << FRAMES7_RX_BUFFER_SIZESHIFT)
static uint8_t Frames7_RxBuffer[FRAMES7_RX_BUFFER_SIZE];
#else
#define FRAMES7_RX_BUFFER_SIZE 0
#define Frames7_RxBuffer NULL
#endif
#if FRAMES7_TX_BUFFER_SIZESHIFT > 0
#define FRAMES7_TX_BUFFER_SIZE (1 << FRAMES7_TX_BUFFER_SIZESHIFT)
static uint8_t Frames7_TxBuffer[FRAMES7_TX_BUFFER_SIZE];
#else
#define FRAMES7_TX_BUFFER_SIZE 0
#define Frames7_TxBuffer NULL
#endif
#undef FRAMES_PERIPHS
#undef FRAMES_IRQS
#undef FRAMES_RX_SIZES
#undef FRAMES_TX_SIZES
#undef FRAMES_RX_BUFFERS
#undef FRAMES_TX_BUFFERS
#define FRAMES_PERIPHS FRAMES0_PERIPH, FRAMES1_PERIPH, FRAMES2_PERIPH, FRAMES3_PERIPH, FRAMES4_PERIPH, FRAMES5_PERIPH, FRAMES6_PERIPH, FRAMES7_PERIPH
#define FRAMES_IRQS FRAMES0_IRQ, FRAMES1_IRQ, FRAMES2_IRQ, FRAMES3_IRQ, FRAMES4_IRQ, FRAMES5_IRQ, FRAMES6_IRQ, FRAMES7_IRQ
#define FRAMES_RX_SIZES FRAMES0_RX_BUFFER_SIZE, FRAMES1_RX_BUFFER_SIZE, FRAMES2_RX_BUFFER_SIZE, FRAMES3_RX_BUFFER_SIZE, FRAMES4_RX_BUFFER_SIZE, FRAMES5_RX_BUFFER_SIZE, FRAMES6_RX_BUFFER_SIZE, FRAMES7_RX_BUFFER_SIZE
#define FRAMES_TX_SIZES FRAMES0_TX_BUFFER_SIZE, FRAMES1_TX_BUFFER_SIZE, FRAMES2_TX_BUFFER_SIZE, FRAMES3_TX_BUFFER_SIZE, FRAMES4_TX_BUFFER_SIZE, FRAMES5_TX_BUFFER_SIZE, FRAMES6_TX_BUFFER_SIZE, FRAMES7_TX_BUFFER_SIZE
#define FRAMES_RX_BUFFERS Frames0_RxBuffer, Frames1_RxBuffer, Frames2_RxBuffer, Frames3_RxBuffer, Frames4_RxBuffer, Frames5_RxBuffer, Frames6_RxBuffer, Frames7_RxBuffer
#define FRAMES_TX_BUFFERS Frames0_TxBuffer, Frames1_TxBuffer, Frames2_TxBuffer, Frames3_TxBuffer, Frames4_TxBuffer, Frames5_TxBuffer, Frames6_TxBuffer, Frames7_TxBuffer

#endif /* defined FRAMES7_PERIPH && defined FRAMES7_IRQ && (FRAMES7_RX_BUFFER_SIZESHIFT > 0 || FRAMES7_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES6_PERIPH && defined FRAMES6_IRQ && (FRAMES6_RX_BUFFER_SIZESHIFT > 0 || FRAMES6_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES5_PERIPH && defined FRAMES5_IRQ && (FRAMES5_RX_BUFFER_SIZESHIFT > 0 || FRAMES5_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES4_PERIPH && defined FRAMES4_IRQ && (FRAMES4_RX_BUFFER_SIZESHIFT > 0 || FRAMES4_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES3_PERIPH && defined FRAMES3_IRQ && (FRAMES3_RX_BUFFER_SIZESHIFT > 0 || FRAMES3_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES2_PERIPH && defined FRAMES2_IRQ && (FRAMES2_RX_BUFFER_SIZESHIFT > 0 || FRAMES2_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES1_PERIPH && defined FRAMES1_IRQ && (FRAMES1_RX_BUFFER_SIZESHIFT > 0 || FRAMES1_TX_BUFFER_SIZESHIFT > 0) */
#endif /* defined FRAMES0_PERIPH && defined FRAMES0_IRQ && (FRAMES0_RX_BUFFER_SIZESHIFT > 0 || FRAMES0_TX_BUFFER_SIZESHIFT > 0) */

#if FRAMES_PORTS > 1
#define portparamlast port
#define portparam portparamlast,
#define portassert(ret) if(port >= FRAMES_PORTS) return ret
#else
#define port 0
#define portparamlast
#define portparam
#define portassert(ret)
#endif

#if defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER

static USART_TypeDef* const Frames_Periphs[FRAMES_PORTS] = { FRAMES_PERIPHS };
static IRQn_Type const Frames_IRQs[FRAMES_PORTS] = { FRAMES_IRQS };
static eFrames_Flags Frames_Flags[FRAMES_PORTS];

void Frames_SetFlagsFromISR(FRAMES_portdecl eFrames_Flags flags) {
	portassert();
	Frames_Flags[port] |= flags;
}

bool Frames_CheckAndResetFlags(FRAMES_portdecl eFrames_Flags flags) {
	portassert(false);
	bool _res;
	NVIC_DisableIRQ(Frames_IRQs[port]); _res = Frames_Flags[port] & flags; Frames_Flags[port] &= ~flags; NVIC_EnableIRQ(Frames_IRQs[port]);
	return _res;
}

#endif /* defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER */

/// ********************************************************************
/// USART Frame Receiver
/// ********************************************************************

#ifdef FRAMES_RECEIVER

typedef enum {
	FRAMES_RECEIVERSTEP_STOP,
	FRAMES_RECEIVERSTEP_ADDRESS,
	FRAMES_RECEIVERSTEP_COMMAND,
	FRAMES_RECEIVERSTEP_SIZE,
	FRAMES_RECEIVERSTEP_DATA,
	FRAMES_RECEIVERSTEP_CRC,
} eFramesRx_Step;

typedef struct {
	eFramesRx_Step Step;
	uint8_t CRC8;
	#ifdef FRAMES_8B
	uint8_t RotationByte;
	uint8_t RotationIndex;
	#endif
	uint8_t FrameAddress;
	uint8_t FrameAddressReady;
	uint8_t FrameCommand;
	uint8_t FrameCommandReady;
	uint8_t FrameSize;
	uint8_t FrameSizeReady;
	uint8_t FrameSizeLeft;
	uint16_t WriteIndex;
	uint16_t ReadIndex;
	uint16_t FrameStartIndex;
	uint16_t volatile FrameCount;
	uint16_t volatile Counter;
} sFrames_RxContext;

static uint16_t const Frames_RxSize[FRAMES_PORTS] = { FRAMES_RX_SIZES };
static uint8_t *const Frames_RxBuffer[FRAMES_PORTS] = { FRAMES_RX_BUFFERS };
static sFrames_RxContext Frames_RxConteces[FRAMES_PORTS];

static void _frames_discard_current_frame(FRAMES_portdecllast) {
	portassert();
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	uint16_t _index_tmp = _rx_ctx->FrameStartIndex;
	_rx_ctx->Counter -= (_rx_ctx->WriteIndex - _index_tmp);
	_rx_ctx->WriteIndex = _index_tmp;
	_rx_ctx->Step = FRAMES_RECEIVERSTEP_STOP;
}

static void _frames_process_receiver_byte(FRAMES_portdecl uint8_t byte) {
	portassert();
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	// address byte
	if(_rx_ctx->Step == FRAMES_RECEIVERSTEP_ADDRESS) {
		_rx_ctx->FrameAddress = byte;
		_rx_ctx->Step++;
	}
	// command byte
	else if(_rx_ctx->Step == FRAMES_RECEIVERSTEP_COMMAND) {
		_rx_ctx->FrameCommand = byte;
		_rx_ctx->Step++;
	}
	// size of the frame
	else if(_rx_ctx->Step == FRAMES_RECEIVERSTEP_SIZE) {
		_rx_ctx->FrameSizeLeft = _rx_ctx->FrameSize = byte;
		_rx_ctx->Step = byte ? FRAMES_RECEIVERSTEP_DATA : FRAMES_RECEIVERSTEP_CRC;
	}
	// useful frame data
	else if(_rx_ctx->Step == FRAMES_RECEIVERSTEP_DATA) {
		if(_rx_ctx->Counter < Frames_RxSize[port]) {
			_rx_ctx->Counter++; _rx_ctx->FrameSizeLeft--; Frames_RxBuffer[port][_rx_ctx->WriteIndex++] = byte;
			if(!_rx_ctx->FrameSizeLeft) _rx_ctx->Step++;
		}
		else { Frames_SetFlagsFromISR(portparam FRAMES_RX_BUFFER_OVERFLOW); _frames_discard_current_frame(portparamlast); }
	}
	// expecting CRC8
	else if (_rx_ctx->Step == FRAMES_RECEIVERSTEP_CRC) {
		if(byte == _rx_ctx->CRC8) {
			// The whole frame has arrived successfully
			if(!_rx_ctx->FrameCount) { _rx_ctx->FrameAddressReady = _rx_ctx->FrameAddress; _rx_ctx->FrameCommandReady = _rx_ctx->FrameCommand; _rx_ctx->FrameSizeReady = _rx_ctx->FrameSize; }
			else {
				uint16_t _index_tmp = _rx_ctx->FrameStartIndex;
				Frames_RxBuffer[port][_index_tmp++] = _rx_ctx->FrameAddress;
				_index_tmp &= (Frames_RxSize[port] - 1);
				Frames_RxBuffer[port][_index_tmp++] = _rx_ctx->FrameCommand;
				_index_tmp &= (Frames_RxSize[port] - 1);
				Frames_RxBuffer[port][_index_tmp] = _rx_ctx->FrameSize;
			}
			_rx_ctx->FrameCount++;
			_rx_ctx->Step = FRAMES_RECEIVERSTEP_STOP;
		}
		else { Frames_SetFlagsFromISR(portparam FRAMES_RX_FRAME_COLLISION); _frames_discard_current_frame(portparamlast); }
	}

	_rx_ctx->WriteIndex &= (Frames_RxSize[port] - 1);
	_rx_ctx->CRC8 = crc8_update(_rx_ctx->CRC8, byte);
}

// Process incomming data to the USART Receiver buffer from interrupt service routine
#ifdef FRAMES_8B
void Frames8b_PushDataFromISR(FRAMES_portdecl uint8_t data) {
	portassert();
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	// Got synchronization data
	if(data & 0x80) {
		if(_rx_ctx->Step != FRAMES_RECEIVERSTEP_STOP) { Frames_SetFlagsFromISR(portparam FRAMES_RX_FRAME_COLLISION); _frames_discard_current_frame(portparamlast); }
		if(_rx_ctx->Counter <= (Frames_RxSize[port] - 3)) {
			_rx_ctx->Counter += 3; _rx_ctx->FrameStartIndex = _rx_ctx->WriteIndex; _rx_ctx->WriteIndex += 3;
			_rx_ctx->Step++; _rx_ctx->CRC8 = 0; _rx_ctx->RotationIndex = 7; _rx_ctx->RotationByte = data & 0x7F;
		}
		else { Frames_SetFlagsFromISR(portparam FRAMES_RX_BUFFER_OVERFLOW); }
	}
	else if(_rx_ctx->Step != FRAMES_RECEIVERSTEP_STOP) {
		uint8_t _rotation_index = _rx_ctx->RotationIndex, _rotation_byte = _rx_ctx->RotationByte;
		uint16_t _data16 = data << _rotation_index;
		_rotation_byte |= (uint8_t)_data16; _rotation_index += 7;
		if(_rotation_index >= 8) {
			_frames_process_receiver_byte(portparam _rotation_byte);
			_rotation_byte = _data16 >> 8; _rotation_index -= 8;
		}
		_rx_ctx->RotationIndex = _rotation_index; _rx_ctx->RotationByte = _rotation_byte;
	}
}
#endif
#ifdef FRAMES_9B
void Frames9b_PushDataFromISR(FRAMES_portdecl uint16_t data) {
	portassert();
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	// Got synchronization data
	if(data & 0x100) {
		if(_rx_ctx->Step != FRAMES_RECEIVERSTEP_STOP) { Frames_SetFlagsFromISR(portparam FRAMES_RX_FRAME_COLLISION); _frames_discard_current_frame(portparamlast); }
		if(_rx_ctx->Counter <= (Frames_RxSize[port] - 3)) {
			_rx_ctx->Counter += 3; _rx_ctx->FrameStartIndex = _rx_ctx->WriteIndex; _rx_ctx->WriteIndex += 3;
			_rx_ctx->Step++; _rx_ctx->CRC8 = 0;
		}
		else { Frames_SetFlagsFromISR(portparam FRAMES_RX_BUFFER_OVERFLOW); }
	}
	_frames_process_receiver_byte(portparam (uint8_t)data);
}
#endif

// Get address of the first available frame in the USART Receiver buffer
uint8_t Frames_GetFrameAddress(FRAMES_portdecllast) {
	portassert(0);
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	return _rx_ctx->FrameCount ? _rx_ctx->FrameAddressReady : 0;
}

// Get command of the first available frame in the USART Receiver buffer
uint8_t Frames_GetFrameCommand(FRAMES_portdecllast) {
	portassert(0);
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	return _rx_ctx->FrameCount ? _rx_ctx->FrameCommandReady : 0;
}

// Get size of the first available frame in the USART Receiver buffer
uint8_t Frames_GetFrameSize(FRAMES_portdecllast) {
	portassert(0);
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	return _rx_ctx->FrameCount ? _rx_ctx->FrameSizeReady : 0;
}

// Get data of the first available frame in the USART Receiver buffer
uint8_t Frames_GetFrameData(FRAMES_portdecl void *dst, uint8_t offset, uint8_t size) {
	portassert(0);
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	if(!_rx_ctx->FrameCount) return 0;
	uint8_t _ret_size = 0;
	minwith(offset, _rx_ctx->FrameSizeReady); minwith(size, _rx_ctx->FrameSizeReady - offset); _ret_size = size;
	uint8_t _src_index = _rx_ctx->ReadIndex + offset + 3;
	uint8_t *_dst = (uint8_t *)dst;
	while(size--) {
		_src_index &= (Frames_RxSize[port] - 1);
		*_dst++ = Frames_RxBuffer[port][_src_index++];
	}
	return _ret_size;
}

// Remove the first available frame from the USART Receiver buffer
void Frames_RemoveFrame(FRAMES_portdecllast) {
	portassert();
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	if(!_rx_ctx->FrameCount) return;
	_rx_ctx->ReadIndex += (_rx_ctx->FrameSizeReady + 3);
	_rx_ctx->ReadIndex &= (Frames_RxSize[port] - 1);
	NVIC_DisableIRQ(Frames_IRQs[port]);
	_rx_ctx->FrameCount--;
	_rx_ctx->Counter -= (_rx_ctx->FrameSizeReady + 3);
	if(_rx_ctx->FrameCount) {
		uint16_t _rx_read_index_tmp = _rx_ctx->ReadIndex;
		_rx_ctx->FrameAddressReady = Frames_RxBuffer[port][_rx_read_index_tmp++];
		_rx_read_index_tmp &= (Frames_RxSize[port] - 1);
		_rx_ctx->FrameCommandReady = Frames_RxBuffer[port][_rx_read_index_tmp++];
		_rx_read_index_tmp &= (Frames_RxSize[port] - 1);
		_rx_ctx->FrameSizeReady = Frames_RxBuffer[port][_rx_read_index_tmp];
	}
	NVIC_EnableIRQ(Frames_IRQs[port]);
}

// Gets a value indicating whether there is a ready frame in the USART Receiver buffer
bool Frames_IsFrameAvailable(FRAMES_portdecllast) {
	portassert(false);
	sFrames_RxContext *_rx_ctx = &Frames_RxConteces[port];

	return _rx_ctx->FrameCount;
}

#endif /* FRAMES_RECEIVER */

/// ********************************************************************
/// USART Frame Transmitter
/// ********************************************************************

#ifdef FRAMES_TRANSMITTER

typedef enum {
	FRAMES_TRANSMITTERSTEP_STOP,
	FRAMES_TRANSMITTERSTEP_ADDRESS,
	FRAMES_TRANSMITTERSTEP_COMMAND,
	FRAMES_TRANSMITTERSTEP_SIZE,
	FRAMES_TRANSMITTERSTEP_DATA,
	FRAMES_TRANSMITTERSTEP_CRC,
} eFramesTx_Step;

typedef enum {
	FRAMES_TXBYTEFLAG_VALID_BYTE = (1 << 0),
	FRAMES_TXBYTEFLAG_SYNCHRONIZE = (1 << 1),
	FRAMES_TXBYTEFLAG_NEED_FLUSH = (1 << 2),
} eFrames_TxByteFlags;

typedef struct {
	eFramesTx_Step Step;
	uint8_t CRC8;
	#ifdef FRAMES_8B
	uint8_t RotationByte;
	uint8_t RotationIndex;
	#endif
	uint8_t FrameSize;
	uint8_t FrameSizeLeft;
	uint16_t WriteIndex;
	uint16_t ReadIndex;
	uint16_t volatile FrameCount;
	uint16_t volatile Counter;
} sFrames_TxContext;

static uint16_t const Frames_TxSize[FRAMES_PORTS] = { FRAMES_TX_SIZES };
static uint8_t *const Frames_TxBuffer[FRAMES_PORTS] = { FRAMES_TX_BUFFERS };
static sFrames_TxContext Frames_TxConteces[FRAMES_PORTS];

static eFrames_TxByteFlags _frames_pick_transmitter_byte(FRAMES_portdecl uint8_t *data) {
	portassert(false);
	sFrames_TxContext *_tx_ctx = &Frames_TxConteces[port];

	uint8_t _byte; eFrames_TxByteFlags _flags = 0; bool _fifo_act = false;
	// try to find new frame
	if(_tx_ctx->Step == FRAMES_TRANSMITTERSTEP_STOP) {
		if(_tx_ctx->FrameCount) { _tx_ctx->Step++; _tx_ctx->CRC8 = 0; }
	}

	// address byte
	if(_tx_ctx->Step == FRAMES_TRANSMITTERSTEP_ADDRESS) {
		_byte = Frames_TxBuffer[port][_tx_ctx->ReadIndex]; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE | FRAMES_TXBYTEFLAG_SYNCHRONIZE; _fifo_act = true;
		_tx_ctx->Step++;
	}
	// command byte
	else if(_tx_ctx->Step == FRAMES_TRANSMITTERSTEP_COMMAND) {
		_byte = Frames_TxBuffer[port][_tx_ctx->ReadIndex]; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE; _fifo_act = true;
		_tx_ctx->Step++;
	}
	// size of the frame
	else if(_tx_ctx->Step == FRAMES_TRANSMITTERSTEP_SIZE) {
		_byte = _tx_ctx->FrameSize = _tx_ctx->FrameSizeLeft = Frames_TxBuffer[port][_tx_ctx->ReadIndex]; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE; _fifo_act = true;
		_tx_ctx->Step = _byte ? FRAMES_TRANSMITTERSTEP_DATA : FRAMES_TRANSMITTERSTEP_CRC;
	}
	// useful frame data
	else if(_tx_ctx->Step == FRAMES_TRANSMITTERSTEP_DATA) {
		_byte = Frames_TxBuffer[port][_tx_ctx->ReadIndex]; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE; _fifo_act = true;
		_tx_ctx->FrameSizeLeft--;
		if(!_tx_ctx->FrameSizeLeft) _tx_ctx->Step++;
	}
	// CRC8
	else if(_tx_ctx->Step == FRAMES_TRANSMITTERSTEP_CRC) {
		_byte = _tx_ctx->CRC8; _flags = FRAMES_TXBYTEFLAG_VALID_BYTE | FRAMES_TXBYTEFLAG_NEED_FLUSH;
		_tx_ctx->FrameCount--;
		_tx_ctx->Step = FRAMES_TRANSMITTERSTEP_STOP;
	}

	if(_fifo_act) { _tx_ctx->ReadIndex = (_tx_ctx->ReadIndex + 1) & (Frames_TxSize[port] - 1); _tx_ctx->Counter--; }
	if(_flags) { *data = _byte; _tx_ctx->CRC8 = crc8_update(_tx_ctx->CRC8, _byte); }
	return _flags;
}

// Process outgoing data from the USART Transmitter buffer from interrupt service routine
#ifdef FRAMES_8B
bool Frames8b_PopDataFromISR(FRAMES_portdecl uint8_t *data) {
	portassert(false);
	sFrames_TxContext *_tx_ctx = &Frames_TxConteces[port];

	bool _ret = false; uint8_t _rotation_index = _tx_ctx->RotationIndex; uint16_t _rotation_byte = _tx_ctx->RotationByte;
	if(_rotation_index >= 7) { _ret = true; *data = _rotation_byte; _rotation_index = _rotation_byte = 0; }
	else {
		uint8_t _data8; eFrames_TxByteFlags _flags = _frames_pick_transmitter_byte(portparam &_data8);
		if(_flags) {
			_ret = true;
			_rotation_byte |= (uint16_t)_data8 << _rotation_index;
			*data = ((_rotation_byte & 0x7F) | ((_flags & FRAMES_TXBYTEFLAG_SYNCHRONIZE) ? 0x80 : 0)); _rotation_index = (_flags & FRAMES_TXBYTEFLAG_NEED_FLUSH) ? 7 : _rotation_index + 1; _rotation_byte >>= 7;
		}
	}
	_tx_ctx->RotationIndex = _rotation_index; _tx_ctx->RotationByte = _rotation_byte;
	return _ret;
}
#endif
#ifdef FRAMES_9B
bool Frames9b_PopDataFromISR(FRAMES_portdecl uint16_t *data) {
	portassert(false);

	uint8_t _data8; eFrames_TxByteFlags _flags = _frames_pick_transmitter_byte(portparam &_data8);
	if(_flags) { *data = (_flags & FRAMES_TXBYTEFLAG_SYNCHRONIZE) ? (_data8 | 0x100) : _data8; }
	return _flags;
}
#endif

// Write a frame to the USART Transmitter buffer
bool Frames_PutFrame(FRAMES_portdecl uint8_t addr, uint8_t cmd, void const *data, uint8_t size) {
	#ifdef FRAMES_EXTENDED
	return (data && size) ? Frames_PutFrameEx(portparam addr, cmd, 1, data, size) : Frames_PutFrameEx(portparam addr, cmd, 0);

	#else /* FRAMES_EXTENDED */
	portassert(false);
	sFrames_TxContext *_tx_ctx = &Frames_TxConteces[port];

	uint16_t _total_size = size + 3;
	if(Frames_TxSize[port] < _total_size) return false;
	while((Frames_TxSize[port] - _tx_ctx->Counter) < _total_size) FRAMES_YIELD();
	Frames_TxBuffer[port][_tx_ctx->WriteIndex++] = addr;
	_tx_ctx->WriteIndex &= (Frames_TxSize[port] - 1);
	Frames_TxBuffer[port][_tx_ctx->WriteIndex++] = cmd;
	_tx_ctx->WriteIndex &= (Frames_TxSize[port] - 1);
	Frames_TxBuffer[port][_tx_ctx->WriteIndex++] = size;
	_tx_ctx->WriteIndex &= (Frames_TxSize[port] - 1);
	uint8_t const *_data_ptr = (uint8_t const *)data; uint8_t _size_cpy = size;
	while(_size_cpy--) {
		Frames_TxBuffer[port][_tx_ctx->WriteIndex++] = *_data_ptr++;
		_tx_ctx->WriteIndex &= (Frames_TxSize[port] - 1);
	}
	NVIC_DisableIRQ(Frames_IRQs[port]);
	_tx_ctx->Counter += _total_size; _tx_ctx->FrameCount++;
	NVIC_EnableIRQ(Frames_IRQs[port]);
	USART_ITConfig(Frames_Periphs[port], USART_IT_TXE, ENABLE);
	return true;

	#endif /* FRAMES_EXTENDED */
}

#ifdef FRAMES_EXTENDED

// Write a frame to the USART Transmitter buffer (extended version)
bool Frames_PutFrameEx(FRAMES_portdecl uint8_t addr, uint8_t cmd, uint32_t datas_cnt, ...) { va_list _ap; va_start(_ap, datas_cnt); bool _res = Frames_PutFrameExVA(portparam addr, cmd, datas_cnt, _ap); va_end(_ap); return _res; }
bool Frames_PutFrameExVA(FRAMES_portdecl uint8_t addr, uint8_t cmd, uint32_t datas_cnt, va_list ap) {
	portassert(false);
	sFrames_TxContext *_tx_ctx = &Frames_TxConteces[port];

	uint16_t _total_size = 3;
	va_list _ap_copy; va_copy(_ap_copy, ap);
	uint32_t _datas_cnt_cpy = datas_cnt;
	while(_datas_cnt_cpy--) { void const *_data = va_arg(_ap_copy, void const *); (void)_data; _total_size += va_arg(_ap_copy, uint32_t); }
	if(Frames_TxSize[port] < _total_size) return false;
	while((Frames_TxSize[port] - _tx_ctx->Counter) < _total_size) FRAMES_YIELD();
	Frames_TxBuffer[port][_tx_ctx->WriteIndex++] = addr;
	_tx_ctx->WriteIndex &= (Frames_TxSize[port] - 1);
	Frames_TxBuffer[port][_tx_ctx->WriteIndex++] = cmd;
	_tx_ctx->WriteIndex &= (Frames_TxSize[port] - 1);
	Frames_TxBuffer[port][_tx_ctx->WriteIndex++] = _total_size - 3;
	_tx_ctx->WriteIndex &= (Frames_TxSize[port] - 1);
	while(datas_cnt--) {
		uint8_t const *_data_ptr = (uint8_t const *)va_arg(ap, uint8_t const *); uint8_t _data_len = va_arg(ap, uint32_t);
		while(_data_len--) {
			Frames_TxBuffer[port][_tx_ctx->WriteIndex++] = *_data_ptr++;
			_tx_ctx->WriteIndex &= (Frames_TxSize[port] - 1);
		}
	}
	NVIC_DisableIRQ(Frames_IRQs[port]);
	_tx_ctx->Counter += _total_size; _tx_ctx->FrameCount++;
	NVIC_EnableIRQ(Frames_IRQs[port]);
	USART_ITConfig(Frames_Periphs[port], USART_IT_TXE, ENABLE);
	return true;
}

#endif /* FRAMES_EXTENDED */

// Waits until all date gone from Transmitter buffer
void Frames_Flush(FRAMES_portdecllast) {
	portassert();
	sFrames_TxContext *_tx_ctx = &Frames_TxConteces[port];

	while(_tx_ctx->FrameCount) FRAMES_YIELD();
}

// Gets a value indicating whether USART Transmitter is free
bool Frames_IsTransmitterFree(FRAMES_portdecllast) {
	portassert(false);
	sFrames_TxContext *_tx_ctx = &Frames_TxConteces[port];

	return !_tx_ctx->FrameCount;
}

#endif /* FRAMES_TRANSMITTER */

#if defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER

// Creates and send request frame
bool Frames_RequestInit(FRAMES_portdecl sFrames_Request *request, uint32_t time_out, uint8_t addr, uint8_t cmd, void const *data, uint8_t size) {
	#ifdef FRAMES_EXTENDED
	return (data && size) ? Frames_RequestInitEx(portparam request, time_out, addr, cmd, 1, data, size) : Frames_RequestInitEx(portparam request, time_out, addr, cmd, 0);

	#else /* FRAMES_EXTENDED */
	portassert(false);

	if(Frames_PutFrame(portparam addr, cmd, data, size)) {
		request->TimeOut = time_out; request->Address = addr; request->Command = cmd;
		return true;
	}
	return false;

	#endif /* FRAMES_EXTENDED */
}

// Send response and waits for response
bool Frames_RequestResponse(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, void const *data, uint8_t size) {
	#ifdef FRAMES_EXTENDED
	return (data && size) ? Frames_RequestResponseEx(portparam timer, time_out, addr, cmd, 1, data, size) : Frames_RequestResponseEx(portparam timer, time_out, addr, cmd, 0);

	#else /* FRAMES_EXTENDED */
	portassert(false);

	sFrames_Request _request;
	if(Frames_RequestInit(portparam &_request, *timer + time_out, addr, cmd, data, size)) {
		while(true) {
			if(Frames_RequestPollForResponse(portparam &_request)) return true;
			if(Frames_RequestCheckForTimeout(portparam &_request, *timer)) return false;
		}
	}
	return false;

	#endif /* FRAMES_EXTENDED */
}

// Send response and waits for response with constant size
bool Frames_RequestResponseWithConstSize(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, void *data_resp, uint8_t resp_size, void const *data, uint8_t size) {
	portassert(false);

	bool _res = false;
	if(Frames_RequestResponse(portparam timer, time_out, addr, cmd, data, size)) {
		uint8_t _resp_size = Frames_GetFrameSize(portparamlast);
		if((_res = (_resp_size == resp_size))) if(data_resp && _resp_size) Frames_GetFrameData(portparam data_resp, 0, _resp_size);
		Frames_RemoveFrame(portparamlast);
	}
	return _res;
}

#ifdef FRAMES_EXTENDED

// Creates and send request frame (extended version)
bool Frames_RequestInitEx(FRAMES_portdecl sFrames_Request *request, uint32_t time_out, uint8_t addr, uint8_t cmd, uint32_t datas_cnt, ...) { va_list _ap; va_start(_ap, datas_cnt); bool _res = Frames_RequestInitExVA(portparam request, time_out, addr, cmd, datas_cnt, _ap); va_end(_ap); return _res; }
bool Frames_RequestInitExVA(FRAMES_portdecl sFrames_Request *request, uint32_t time_out, uint8_t addr, uint8_t cmd, uint32_t datas_cnt, va_list ap) {
	if(Frames_PutFrameExVA(portparam addr, cmd, datas_cnt, ap)) {
		request->TimeOut = time_out; request->Address = addr; request->Command = cmd;
		return true;
	}
	return false;
}

// Send response and waits for response (extended version)
bool Frames_RequestResponseEx(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, uint32_t datas_cnt, ...) { va_list _ap; va_start(_ap, datas_cnt); bool _res = Frames_RequestResponseExVA(portparam timer, time_out, addr, cmd, datas_cnt, _ap); va_end(_ap); return _res; }
bool Frames_RequestResponseExVA(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, uint32_t datas_cnt, va_list ap) {
	portassert(false);

	sFrames_Request _request;
	if(Frames_RequestInitExVA(portparam &_request, *timer + time_out, addr, cmd, datas_cnt, ap)) {
		while(true) {
			if(Frames_RequestPollForResponse(portparam &_request)) return true;
			if(Frames_RequestCheckForTimeout(portparam &_request, *timer)) return false;
		}
	}
	return false;
}

// Send response and waits for response with constant size (extended version)
bool Frames_RequestResponseWithConstSizeEx(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, void *data_resp, uint8_t resp_size, uint32_t datas_cnt, ...) { va_list _ap; va_start(_ap, datas_cnt); bool _res = Frames_RequestResponseWithConstSizeExVA(portparam timer, time_out, addr, cmd, data_resp, resp_size, datas_cnt, _ap); va_end(_ap); return _res; }
bool Frames_RequestResponseWithConstSizeExVA(FRAMES_portdecl uint32_t volatile *timer, uint32_t time_out, uint8_t addr, uint8_t cmd, void *data_resp, uint8_t resp_size, uint32_t datas_cnt, va_list ap) {
	portassert(false);

	bool _res = false;
	if(Frames_RequestResponseExVA(portparam timer, time_out, addr, cmd, datas_cnt, ap)) {
		uint8_t _resp_size = Frames_GetFrameSize(portparamlast);
		if((_res = (_resp_size == resp_size))) if(data_resp && _resp_size) Frames_GetFrameData(portparam data_resp, 0, _resp_size);
		Frames_RemoveFrame(portparamlast);
	}
	return _res;
}

#endif /* FRAMES_EXTENDED */

// Polls for the response for specified request
bool Frames_RequestPollForResponse(FRAMES_portdecl sFrames_Request const *request) {
	portassert(false);

	return Frames_IsFrameAvailable(portparamlast) && Frames_GetFrameAddress(portparamlast) == request->Address && Frames_GetFrameCommand(portparamlast) == request->Command;
}

// Polls for the response for specified request
bool Frames_RequestCheckForTimeout(FRAMES_portdecl sFrames_Request const *request, uint32_t timer) {
	portassert(false);

	return ((int32_t)(request->TimeOut - timer)) <= 0;
}

#endif /* defined FRAMES_RECEIVER || defined FRAMES_TRANSMITTER */

#endif /* defined FRAMES_8B || defined FRAMES_9B */
