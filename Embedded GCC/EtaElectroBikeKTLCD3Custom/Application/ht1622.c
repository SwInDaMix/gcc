#include "ht1622.h"
#include "periph.h"

typedef enum {
    HT1622_BITS_X = 1,
    HT1622_BITS_ID = 3,
    HT1622_BITS_ADDRESS = 6,
    HT1622_BITS_DATA = 8
} eHT1622_Bits;

typedef enum {
    HT1622_ID_CMD = 4,
    HT1622_ID_DATA = 5
} eHT1622_ID;

typedef enum {
    HT1622_CMD_SYS_DIS      = 0x00,  // SYS DIS    (0000-0000-X) Turn off system oscillator, LCD bias gen [Default]
    HT1622_CMD_SYS_EN       = 0x01,  // SYS EN     (0000-0001-X) Turn on  system oscillator
    HT1622_CMD_LCD_OFF      = 0x02,  // LCD OFF    (0000-0010-X) Turn off LCD display [Default]
    HT1622_CMD_LCD_ON       = 0x03,  // LCD ON     (0000-0011-X) Turn on  LCD display
    HT1622_CMD_RC_INT       = 0x10   // RC INT     (0001-10XX-X) System clock source, on-chip RC oscillator
} eHT1622_Command;

#define delay()
//static void delay() {
//    uint16_t _i;
//    for (_i = 0; _i < 1000; ++_i) {
//        nop();
//    }
//}

static void ht1622_send_bits(uint16_t data, eHT1622_Bits bits) {
    uint16_t _mask = 1 << (bits - 1);
    while ((bits--)) {
        periph_set_ht1622_data(data & _mask);
        //delay();
        periph_set_ht1622_write(false);
        //delay();
        periph_set_ht1622_write(true);
        //delay();
        _mask >>= 1;
    }
}

static void ht1622_send_command(eHT1622_Command command) {
    periph_set_ht1622_cs(false);
    delay();
    ht1622_send_bits(HT1622_ID_CMD, HT1622_BITS_ID);
    ht1622_send_bits(command, HT1622_BITS_DATA);
    ht1622_send_bits(0, HT1622_BITS_X);
    periph_set_ht1622_cs(true);
    delay();
}

void ht1622_init() {
    periph_set_ht1622_cs(true);
    periph_set_ht1622_write(true);
    periph_set_ht1622_read(true);
    periph_set_ht1622_data(true);
    periph_set_ht1622_vdd(true);

    ht1622_send_command(HT1622_CMD_RC_INT);
    ht1622_send_command(HT1622_CMD_SYS_EN);
    ht1622_send_command(HT1622_CMD_LCD_ON);
}

void ht1622_send_frame(sHT1622Frame const *buffer) {
    uint8_t *buffer_ptr = buffer->buffer;
    uint8_t cnt = sizeof(buffer->buffer);

    periph_set_ht1622_cs(false);
    delay();
    ht1622_send_bits(HT1622_ID_DATA, HT1622_BITS_ID);
    ht1622_send_bits(0, HT1622_BITS_ADDRESS);

    while ((cnt--)) {
        ht1622_send_bits(*buffer_ptr++, HT1622_BITS_DATA);
    }

    periph_set_ht1622_cs(true);
    delay();
}
