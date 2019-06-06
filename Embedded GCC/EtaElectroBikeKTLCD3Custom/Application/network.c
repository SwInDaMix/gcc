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
    NetworkState__Max,
} eNetworkState;

static uint8_t s_network_rx_buffer[NETWORK_RX_BUFFER_SIZE];
static volatile uint8_t s_network_rx_head_idx;
static volatile uint8_t s_network_rx_size;

static sNetworkPayload_LCD s_network_rx_payload_lcd;

static sNetworkPayload_Controller s_network_tx_payload_controller;
static sNetworkPayload_Controller s_network_tx_payload_controller_preload;
static volatile uint8_t s_network_tx_payload_controller_crc;
static volatile uint8_t s_network_tx_payload_controller_crc_preload;
static volatile bool s_network_tx_payload_controller_ready;

static void network_uart_on_received(uint8_t byte) {
    if(s_network_rx_size < NETWORK_RX_BUFFER_SIZE) {
        s_network_rx_buffer[s_network_rx_head_idx] = byte;
        s_network_rx_size++;
        s_network_rx_head_idx++; if(s_network_rx_head_idx >= NETWORK_RX_BUFFER_SIZE) s_network_rx_head_idx = 0;
    } // else buffer overflow
}

static uint8_t network_uart_on_transmit() {
    static eNetworkState _s_state;
    static uint8_t _s_size;
    static uint8_t const *_s_payload_ptr;
    uint8_t _byte = 0;

    if(s_network_tx_payload_controller_ready) {
        memcpy(&s_network_tx_payload_controller, &s_network_tx_payload_controller_preload, sizeof(sNetworkPayload_Controller));
        s_network_tx_payload_controller_crc = s_network_tx_payload_controller_crc_preload;
        s_network_tx_payload_controller_ready = false;
        _s_state = NetworkState_Pream1;
    }

    if(_s_state == NetworkState_Pream1) { _byte = NETWORK_PREAMBULA_B1; _s_state = NetworkState_Pream2; }
    else if(_s_state == NetworkState_Pream2) { _byte = NETWORK_PREAMBULA_B2; _s_state = NetworkState_Pream3; }
    else if(_s_state == NetworkState_Pream3) { _byte = NETWORK_PREAMBULA_B3; _s_state = NetworkState_Pream4; }
    else if(_s_state == NetworkState_Pream4) { _byte = NETWORK_PREAMBULA_B4; _s_state = NetworkState_Cmd; }
    else if(_s_state == NetworkState_Cmd) { _byte = NetworkCmd_Controller; _s_payload_ptr = (uint8_t const *)&s_network_tx_payload_controller; _s_size = sizeof(sNetworkPayload_Controller); _s_state = NetworkState_Size; }
    else if(_s_state == NetworkState_Size) { _byte = _s_size; _s_state = _s_size ? NetworkState_Payload : NetworkState_CRC; }
    else if(_s_state == NetworkState_Payload) { _byte = *_s_payload_ptr++; _s_size--; if(!_s_size) _s_state = NetworkState_CRC; }
    else if(_s_state == NetworkState_CRC) { _byte = s_network_tx_payload_controller_crc; _s_state = NetworkState_Pream1; }

    return _byte;
}

void network_cycle() {
    static eNetworkState _s_state;
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
        else if(_s_state == NetworkState_Size && _byte == _s_size) { _s_crc = crc8_ccitt_update(0, _byte); _s_size_left = _s_size; _s_state = _s_size ? NetworkState_Payload : NetworkState_CRC; }
        else if(_s_state == NetworkState_Payload) { _s_crc = crc8_ccitt_update(_s_crc, _byte); _s_size_left--; if(!_s_size_left) _s_state = NetworkState_CRC; }
        else if(_s_state == NetworkState_CRC) { _s_state = _s_crc == _byte ? NetworkState_Packet : NetworkState_Pream1; }

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
                // no payload or consumer, so simply skip the packet
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
    periph_uart_set_on_received_callback(network_uart_on_received);
    periph_uart_set_on_transmit_callback(network_uart_on_transmit);
#endif

    s_network_rx_payload_lcd.control.max_gear = 3;
    s_network_rx_payload_lcd.control.battery_soc = NetworkBatterySoC_Recuperating;
    s_network_rx_payload_lcd.sensors.temp_motor = 25;
    s_network_rx_payload_lcd.sensors.voltage = 23456;
    s_network_rx_payload_lcd.sensors.wattage = 455;
    s_network_rx_payload_lcd.sensors.erps = 150;

    s_network_rx_payload_lcd.stat.erotations = 150000;
    s_network_rx_payload_lcd.stat.ride_time = 720;
    s_network_rx_payload_lcd.stat.wattage_consumed = 1234;
}

sNetworkPayload_LCD const *network_get_payload_lcd() { return &s_network_rx_payload_lcd; }

void network_update_payload_controller(sNetworkPayload_Controller const *payload_controller) {
    uint8_t _crc, _cnt = sizeof(sNetworkPayload_Controller);
    uint8_t const *_ptr = (uint8_t const *)&s_network_tx_payload_controller_preload;
    _crc = crc8_ccitt_update(0, NetworkCmd_Controller);
    _crc = crc8_ccitt_update(_crc, sizeof(sNetworkPayload_Controller));
    while((_cnt--)) _crc = crc8_ccitt_update(_crc, *_ptr++);

    if(s_network_tx_payload_controller_crc_preload != _crc) {
        s_network_tx_payload_controller_ready = false;
        memcpy(&s_network_tx_payload_controller_preload, payload_controller, sizeof(sNetworkPayload_Controller));
        s_network_tx_payload_controller_crc_preload = _crc;
        s_network_tx_payload_controller_ready = true;
    }
}
