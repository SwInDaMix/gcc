#include <string.h>
#include <network.h>

#include "network.h"
#include "periph.h"
#include "utils.h"

#define NETWORK_RX_BUFFER_SIZE 128

#define NETWORK_PREAMBULA_B1 (NETWORK_PREAMBULA & 0xFF)
#define NETWORK_PREAMBULA_B2 ((NETWORK_PREAMBULA >> 8) & 0xFF)
#define NETWORK_PREAMBULA_B3 ((NETWORK_PREAMBULA >> 16) & 0xFF)
#define NETWORK_PREAMBULA_B4 ((NETWORK_PREAMBULA >> 24) & 0xFF)

#define NETWORK_PACKET_HEADER_SIZE (4 + 2)
#define NETWORK_PACKET_OVERHEAD (NETWORK_PACKET_HEADER_SIZE + 1)

typedef enum {
    NetworkState_Pream1,
    NetworkState_Pream2,
    NetworkState_Pream3,
    NetworkState_Pream4,
    NetworkState_Cmd,
    NetworkState_Size,
    NetworkState_Payload,
    NetworkState_CRC,
    NetworkState_Packet,
} eNetworkState;

static uint8_t s_network_rx_buffer[NETWORK_RX_BUFFER_SIZE];
static volatile uint8_t s_network_rx_head_idx;
static volatile uint8_t s_network_rx_size;

static sNetworkPayload_LCD s_network_rx_payload_lcd;

static sNetworkPayload_Controller s_network_tx_payload_controller;
static sNetworkPayload_Controller s_network_tx_payload_controller_preload;
static bool s_network_tx_payload_ready;

static void network_uart_byte_receiver(uint8_t byte) {
    if(s_network_rx_size < NETWORK_RX_BUFFER_SIZE) {
        s_network_rx_buffer[s_network_rx_head_idx] = byte;
        s_network_rx_size++;
        s_network_rx_head_idx++; if(s_network_rx_head_idx >= NETWORK_RX_BUFFER_SIZE) s_network_rx_head_idx = 0;
    } // else buffer overflow
}

static void network_send_packet(eNetworkCmd cmd, uint8_t size, void const *payload) {
    uint8_t _crc = 0;
    uint8_t const *_ptr = (uint8_t const *)payload;
    periph_uart_putbyte(NETWORK_PREAMBULA_B1);
    periph_uart_putbyte(NETWORK_PREAMBULA_B2);
    periph_uart_putbyte(NETWORK_PREAMBULA_B3);
    periph_uart_putbyte(NETWORK_PREAMBULA_B4);
    periph_uart_putbyte(cmd); _crc = crc8_ccitt_update(0, cmd);
    periph_uart_putbyte(size); _crc = crc8_ccitt_update(_crc, size);
    while((size--)) {
        uint8_t _byte = *_ptr++;
        periph_uart_putbyte(_byte); _crc = crc8_ccitt_update(_crc, _byte);
    }
    periph_uart_putbyte(_crc);
}

void network_cycle() {
    static eNetworkState _s_state = NetworkState_Pream1;
    static uint8_t _s_rx_idx_pream1;   // start of pream1 index
    static uint8_t _s_rx_idx;          // current observation index
    static eNetworkCmd _s_cmd;
    static eNetworkCmd _s_size;
    static uint8_t _s_size_left, _s_crc;

    while(_s_rx_idx != s_network_rx_head_idx) {
        uint8_t _byte = s_network_rx_buffer[_s_rx_idx];
        _s_rx_idx++; if(_s_rx_idx >= NETWORK_RX_BUFFER_SIZE) _s_rx_idx = 0;

        if(_s_state == NetworkState_Pream1 && _byte == NETWORK_PREAMBULA_B1) _s_state = NetworkState_Pream2;
        else if(_s_state == NetworkState_Pream2 && _byte == NETWORK_PREAMBULA_B2) _s_state = NetworkState_Pream3;
        else if(_s_state == NetworkState_Pream3 && _byte == NETWORK_PREAMBULA_B3) _s_state = NetworkState_Pream4;
        else if(_s_state == NetworkState_Pream4 && _byte == NETWORK_PREAMBULA_B4) _s_state = NetworkState_Cmd;
        else if(_s_state == NetworkState_Cmd) {
            _s_state = NetworkState_Size;
            if(_byte == NetworkCmd_LCD) _s_size = sizeof(sNetworkPayload_LCD);
            else _s_state = NetworkState_Pream1;
        }
        else if(_s_state == NetworkState_Size && _byte == _s_size) { _s_size_left = _s_size; _s_crc = crc8_ccitt_update(_s_crc, _s_size); _s_state = _s_size ? NetworkState_Payload : NetworkState_CRC; }
        else if(_s_state == NetworkState_Payload) { _s_crc = crc8_ccitt_update(_s_crc, _byte); _s_size_left--; if(!_s_size_left) _s_state = NetworkState_CRC; }
        else if(_s_state == NetworkState_CRC) { _s_state = _s_crc == _byte ? NetworkState_Packet : NetworkState_Pream1; }
        else _s_state = NetworkState_Pream1;

        if(_s_state == NetworkState_Packet) {
            uint8_t *_dst = NULL;
            if(_s_cmd == NetworkCmd_LCD) { _dst = (uint8_t *)&s_network_rx_payload_lcd; }

            if(_dst != NULL) {
                // skip preambula, cmd and size bytes
                _s_rx_idx_pream1 += NETWORK_PACKET_HEADER_SIZE; if(_s_rx_idx_pream1 >= NETWORK_RX_BUFFER_SIZE) _s_rx_idx_pream1 = 0;
                // copy payload
                _s_size_left = _s_size;
                while((_s_size_left--)) {
                    *_dst++ = s_network_rx_buffer[_s_rx_idx_pream1];
                    _s_rx_idx_pream1++; if(_s_rx_idx_pream1 >= NETWORK_RX_BUFFER_SIZE) _s_rx_idx_pream1 = 0;
                }
                // skip crc byte
                _s_rx_idx_pream1++; if(_s_rx_idx_pream1 >= NETWORK_RX_BUFFER_SIZE) _s_rx_idx_pream1 = 0;
            } else {
                // no payload or consumer, so simple skip the packet
                _s_rx_idx_pream1 += (NETWORK_PACKET_OVERHEAD + _s_size); if(_s_rx_idx_pream1 >= NETWORK_RX_BUFFER_SIZE) _s_rx_idx_pream1 = 0;
            }
            // decrease the size of occupied buffer
            periph_atom_start();
            s_network_rx_size -= (NETWORK_PACKET_OVERHEAD + _s_size);
            periph_atom_end();
        } else if(_s_state == NetworkState_Pream1) {
            _s_rx_idx_pream1++; if(_s_rx_idx_pream1 >= NETWORK_RX_BUFFER_SIZE) _s_rx_idx_pream1 = 0;
            periph_atom_start();
            s_network_rx_size--;
            periph_atom_end();
            _s_rx_idx = _s_rx_idx_pream1;
        }
    }
}

void network_init() {
#ifndef DEBUG
    periph_uart_set_on_received_callback(network_uart_byte_receiver);
#endif
}

sNetworkPayload_LCD const *network_get_payload_lcd() { return &s_network_rx_payload_lcd; }

void network_send_payload_controller(sNetworkPayload_Controller const *payload_controller) { network_send_packet(NetworkCmd_Controller, sizeof(sNetworkPayload_Controller), payload_controller); }
