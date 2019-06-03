#include <string.h>

#include "lcd.h"
#include "ht1622.h"
#include "utils.h"

#define LCD_BATTERYSOC_OFFSET 23

typedef enum {
    LCDBitInternal_Gear_Cruise = 0,
    LCDBitInternal_Gear_Assist = 15,
    LCDBitInternal_ODOVolts_Vol = 23,
    LCDBitInternal_ODOVolts_ODO = 31,
    LCDBitInternal_ODOVolts_Km = 39,
    LCDBitInternal_ODOVolts_Mil = 47,
    LCDBitInternal_ODOVolts_Dot = 55,
    LCDBitInternal_Temp_One = 63,
    LCDBitInternal_Temp_Minus = 71,
    LCDBitInternal_Temp_C = 72,
    LCDBitInternal_Temp_F = 73,
    LCDBitInternal_PowerTemp_C = 74,
    LCDBitInternal_PowerTemp_W = 75,
    LCDBitInternal_PowerTemp_F = 76,
    LCDBitInternal_PowerTemp_Motor = 77,
    LCDBitInternal_PowerTemp_One = 103,
    LCDBitInternal_Speed_AVS = 104,
    LCDBitInternal_Speed_MXS = 105,
    LCDBitInternal_Speed_Walking = 106,
    LCDBitInternal_Speed_Mph = 107,
    LCDBitInternal_Speed_Kmh = 108,
    LCDBitInternal_Speed_Dot = 111,
    LCDBitInternal_ODOVolts_DST = 135,
    LCDBitInternal_TravelTime_TM = 136,
    LCDBitInternal_TravelTime_TTM = 137,
    LCDBitInternal_Light = 189,
    LCDBitInternal_Braking = 190,
    LCDBitInternal_TravelTime_Colon = 191
} eLCDBitInternal;

typedef enum {
    LCDDigitOffsetInternal__Twisted        = 0x80,
    LCDDigitOffsetInternal_Gear     = 1,
    LCDDigitOffsetInternal_ODOVolts_1      = 2,
    LCDDigitOffsetInternal_ODOVolts_2      = 3,
    LCDDigitOffsetInternal_ODOVolts_3      = 4,
    LCDDigitOffsetInternal_ODOVolts_4      = 5,
    LCDDigitOffsetInternal_ODOVolts_5      = 6,
    LCDDigitOffsetInternal_Temp_1          = 7,
    LCDDigitOffsetInternal_Temp_2          = 8,
    LCDDigitOffsetInternal_PowerTemp_3     = 10 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_PowerTemp_2     = 11 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_PowerTemp_1     = 12 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_Speed_3         = 14 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_Speed_2         = 15 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_Speed_1         = 16 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_TravelTime_5    = 18 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_TravelTime_4    = 19 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_TravelTime_3    = 20 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_TravelTime_2    = 21 | LCDDigitOffsetInternal__Twisted,
    LCDDigitOffsetInternal_TravelTime_1    = 22 | LCDDigitOffsetInternal__Twisted
} eLCDDigitOffsetInternal;

typedef enum {
    //                                      654 3210
    LCDDigitInternal_None       = 0x00, // 0000 0000
    LCDDigitInternal_0          = 0x77, // 0111 0111
    LCDDigitInternal_1          = 0x24, // 0010 0100
    LCDDigitInternal_2          = 0x6B, // 0110 1011
    LCDDigitInternal_3          = 0x6D, // 0110 1101
    LCDDigitInternal_4          = 0x3C, // 0011 1100
    LCDDigitInternal_5          = 0x5D, // 0101 1101
    LCDDigitInternal_6          = 0x5F, // 0101 1111
    LCDDigitInternal_7          = 0x74, // 0111 0100
    LCDDigitInternal_8          = 0x7F, // 0111 1111
    LCDDigitInternal_9          = 0x7D, // 0111 1101
    LCDDigitInternal_A          = 0x7E, // 0111 1110
    LCDDigitInternal_b          = 0x1F, // 0001 1111
    LCDDigitInternal_C          = 0x53, // 0101 0011
    LCDDigitInternal_d          = 0x2F, // 0010 1111
    LCDDigitInternal_E          = 0x5B, // 0101 1011
    LCDDigitInternal_F          = 0x5A, // 0101 1010
    LCDDigitInternal_Minus      = 0x08, // 0000 1000
    LCDDigitInternal_Minus1   = 0x2C, // 0010 1100
    LCDDigitInternal_c          = 0x0B, // 0000 1011
    LCDDigitInternal_n          = 0x0E, // 0000 1110
    LCDDigitInternal_P          = 0x7A, // 0111 1010
    LCDDigitInternal_o          = 0x0F, // 0000 1111
    LCDDigitInternal_r          = 0x0A, // 0000 1010
    LCDDigitInternal_u          = 0x07, // 0000 0111
    LCDDigitInternal_ROT0       = 0x40, // 0100 0000
    LCDDigitInternal_ROT1       = 0x20, // 0010 0000
    LCDDigitInternal_ROT2       = 0x04, // 0000 0100
    LCDDigitInternal_ROT3       = 0x01, // 0000 0001
    LCDDigitInternal_ROT4       = 0x02, // 0000 0010
    LCDDigitInternal_ROT5       = 0x10, // 0001 0000
    LCDDigitInternal_ROTD0      = 0x60, // 0110 0000
    LCDDigitInternal_ROTD1      = 0x24, // 0010 0100
    LCDDigitInternal_ROTD2      = 0x05, // 0000 0101
    LCDDigitInternal_ROTD3      = 0x03, // 0000 0011
    LCDDigitInternal_ROTD4      = 0x12, // 0001 0010
    LCDDigitInternal_ROTD5      = 0x50  // 0101 0000
} eLCDDigitInternal;

static eLCDBitInternal const s_lcd_bit_enums[LCDBit__Max] = {
    LCDBitInternal_Gear_Cruise,
    LCDBitInternal_Gear_Assist,
    LCDBitInternal_ODOVolts_Vol,
    LCDBitInternal_ODOVolts_ODO,
    LCDBitInternal_ODOVolts_Km,
    LCDBitInternal_ODOVolts_Mil,
    LCDBitInternal_ODOVolts_Dot,
    LCDBitInternal_Temp_One,
    LCDBitInternal_Temp_Minus,
    LCDBitInternal_Temp_C,
    LCDBitInternal_Temp_F,
    LCDBitInternal_PowerTemp_C,
    LCDBitInternal_PowerTemp_W,
    LCDBitInternal_PowerTemp_F,
    LCDBitInternal_PowerTemp_Motor,
    LCDBitInternal_PowerTemp_One,
    LCDBitInternal_Speed_AVS,
    LCDBitInternal_Speed_MXS,
    LCDBitInternal_Speed_Walking,
    LCDBitInternal_Speed_Mph,
    LCDBitInternal_Speed_Kmh,
    LCDBitInternal_Speed_Dot,
    LCDBitInternal_ODOVolts_DST,
    LCDBitInternal_TravelTime_TM,
    LCDBitInternal_TravelTime_TTM,
    LCDBitInternal_Light,
    LCDBitInternal_Braking,
    LCDBitInternal_TravelTime_Colon,
};
static eLCDDigitOffsetInternal const s_lcd_digit_offset_enums[LCDDigitOffset__Max] = {
    LCDDigitOffsetInternal_Gear,
    LCDDigitOffsetInternal_ODOVolts_1,
    LCDDigitOffsetInternal_ODOVolts_2,
    LCDDigitOffsetInternal_ODOVolts_3,
    LCDDigitOffsetInternal_ODOVolts_4,
    LCDDigitOffsetInternal_ODOVolts_5,
    LCDDigitOffsetInternal_Temp_1,
    LCDDigitOffsetInternal_Temp_2,
    LCDDigitOffsetInternal_PowerTemp_3,
    LCDDigitOffsetInternal_PowerTemp_2,
    LCDDigitOffsetInternal_PowerTemp_1,
    LCDDigitOffsetInternal_Speed_3,
    LCDDigitOffsetInternal_Speed_2,
    LCDDigitOffsetInternal_Speed_1,
    LCDDigitOffsetInternal_TravelTime_5,
    LCDDigitOffsetInternal_TravelTime_4,
    LCDDigitOffsetInternal_TravelTime_3,
    LCDDigitOffsetInternal_TravelTime_2,
    LCDDigitOffsetInternal_TravelTime_1
};
static eLCDDigitInternal const s_lcd_digit_enums[LCDDigit__Max] = {
    LCDDigitInternal_None,
    LCDDigitInternal_0,
    LCDDigitInternal_1,
    LCDDigitInternal_2,
    LCDDigitInternal_3,
    LCDDigitInternal_4,
    LCDDigitInternal_5,
    LCDDigitInternal_6,
    LCDDigitInternal_7,
    LCDDigitInternal_8,
    LCDDigitInternal_9,
    LCDDigitInternal_A,
    LCDDigitInternal_b,
    LCDDigitInternal_C,
    LCDDigitInternal_d,
    LCDDigitInternal_E,
    LCDDigitInternal_F,
    LCDDigitInternal_Minus,
    LCDDigitInternal_Minus1,
    LCDDigitInternal_c,
    LCDDigitInternal_n,
    LCDDigitInternal_P,
    LCDDigitInternal_o,
    LCDDigitInternal_r,
    LCDDigitInternal_u,
    LCDDigitInternal_ROT0,
    LCDDigitInternal_ROT1,
    LCDDigitInternal_ROT2,
    LCDDigitInternal_ROT3,
    LCDDigitInternal_ROT4,
    LCDDigitInternal_ROT5,
    LCDDigitInternal_ROTD0,
    LCDDigitInternal_ROTD1,
    LCDDigitInternal_ROTD2,
    LCDDigitInternal_ROTD3,
    LCDDigitInternal_ROTD4,
    LCDDigitInternal_ROTD5
};

static sHT1622Frame s_lcd_shadow_frame;

static eLCDDigitInternal lcd_twist_digit(eLCDDigitInternal digit) {
    uint8_t orig = (uint8_t)digit;
    uint8_t res = 0;
    uint8_t cnt = 7;

    while((cnt--)) {
        res <<= 1;
        if(orig & 1) res |= 1;
        orig >>= 1;
    }

    return (eLCDDigitInternal)res;
}

void lcd_flush() { ht1622_send_frame(&s_lcd_shadow_frame); }
void lcd_clear() { memset(&s_lcd_shadow_frame.buffer, 0, sizeof(s_lcd_shadow_frame.buffer)); }

void lcd_set_bit(eLCDBit bit, bool value) {
    eLCDBitInternal bit_internal = s_lcd_bit_enums[bit];
    uint8_t *ptr = &s_lcd_shadow_frame.buffer[(uint8_t)bit_internal >> 3];
    uint8_t b = (uint8_t)bit_internal & 7;

    if(value) *ptr =  *ptr | (1 << b);
    else *ptr =  *ptr & ~(1 << b);
}
void lcd_set_digit(eLCDDigitOffset digit_offset, eLCDDigit digit) {
    eLCDDigitOffsetInternal digit_offset_internal = s_lcd_digit_offset_enums[digit_offset];
    eLCDDigitInternal digit_internal = s_lcd_digit_enums[digit];
    uint8_t *ptr;

    if(digit_offset_internal & LCDDigitOffsetInternal__Twisted) {
        digit_offset_internal &= ~(LCDDigitOffsetInternal__Twisted);
        digit_internal = lcd_twist_digit(digit_internal);
    }
    ptr = &s_lcd_shadow_frame.buffer[digit_offset_internal];

    *ptr = *ptr & 0x80 | digit_internal;
}
void lcd_set_battery_soc_bits(eLCDBatterySocBits battery_soc_bits) {
    uint8_t *ptr = &s_lcd_shadow_frame.buffer[LCD_BATTERYSOC_OFFSET];

    *ptr = *ptr & ~LCDBatterySocBit__All | battery_soc_bits;
}
