#include <string.h>
#include "lcd.h"
#include "ht1622.h"
#include "utils.h"

#define LCD_BATTERYSOC_OFFSET 23

static sHT1622Frame s_lcd_shadow_frame;

static eLCDDigit lcd_twist_digit(eLCDDigit digit) {
    uint8_t orig = (uint8_t)digit;
    uint8_t res = 0;
    uint8_t cnt = 6;

    while ((cnt--)) {
        if(orig & 1) res |= 0x40;
        orig >>= 1;
        res >>= 1;
    }

    return (eLCDDigit)res;
}

void lcd_flush() { ht1622_send_frame(&s_lcd_shadow_frame); }
void lcd_clear() { memset(&s_lcd_shadow_frame.buffer, 0, sizeof(s_lcd_shadow_frame.buffer)); }

void lcd_set_bit(eLCDBit bit, bool value) {
    uint8_t *ptr = &s_lcd_shadow_frame.buffer[(uint8_t)bit >> 3];
    uint8_t b = (uint8_t)bit & 7;

    if(value) *ptr =  *ptr | (1 << b);
    else *ptr =  *ptr & ~(1 << b);
}

void lcd_set_digit(eLCDDigitOffset digit_offset, eLCDDigit digit) {
    uint8_t *ptr;

    if(digit_offset & LCDDigitOffset__Twisted) {
        digit_offset &= ~(LCDDigitOffset__Twisted);
        digit = lcd_twist_digit(digit);
    }
    ptr = &s_lcd_shadow_frame.buffer[digit_offset];

    *ptr = *ptr & 0x80 | digit;
}

void lcd_set_battery_soc_bits(eLCDBatterySocBits battery_soc_bits) {
    uint8_t *ptr = &s_lcd_shadow_frame.buffer[LCD_BATTERYSOC_OFFSET];

    *ptr = *ptr & ~LCDBatterySoC__All | battery_soc_bits;
}