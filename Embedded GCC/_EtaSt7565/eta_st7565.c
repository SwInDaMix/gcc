/********************* (C) 2015 Eta Software House. ********************
 Author    : Sw
 File Name : eta_st7565.c
 ***********************************************************************/

#include "stdperiph.h"
#include "config.h"
#include "eta_st7565.h"
#include "eta_delay.h"
#include "eta_crc.h"

static sGLCD_Interface const *g_lcd_interface;

static bit g_rotate_180;
static uint32_t g_lcd_screen_rows_crc[GLCD_HEIGHT >> 3];

__attribute__((aligned(4)))
static uint8_t g_lcd_screen[(GLCD_WIDTH * GLCD_HEIGHT) >> 3];

static void _glcd_write_data(uint8_t data) {
    g_lcd_interface->a0(true);
    g_lcd_interface->spi_write(data);
}

static void _glcd_write_cmd(eGLCD_Cmd cmd) {
    g_lcd_interface->a0(false);
    g_lcd_interface->spi_write(cmd);
}

static void _glcd_set_row(uint8_t row) {
    _glcd_write_cmd(GLCD_Cmd_SetPage | (row & 0xF));
}

static void _glcd_set_column(uint8_t column) {
    if (g_rotate_180) column += 4;
    _glcd_write_cmd(GLCD_Cmd_SetColumnLower | (column & 0xF));
    _glcd_write_cmd(GLCD_Cmd_SetColumnUpper | (column >> 4));
}

uint8_t *GLCD_GetScreen() {
    return g_lcd_screen;
}

void GLCD_Flush() {
    uint32_t *_row_crc = g_lcd_screen_rows_crc;
    uint8_t *_src = g_lcd_screen;
    for (uint8_t rows = 0; rows < GLCD_HEIGHT >> 3; rows++) {
        uint32_t _crc_new = crc32_fast(UINT32_MAX, _src, GLCD_WIDTH);
        if (*_row_crc != _crc_new) {
            *_row_crc = _crc_new;
            g_lcd_interface->spi_cs(true);
            _glcd_set_row(rows);
            _glcd_set_column(0);
            uint8_t cols = GLCD_WIDTH;
            while (cols--) _glcd_write_data(*_src++);
            g_lcd_interface->spi_cs(false);
        } else {
            _src += GLCD_WIDTH;
        }
        _row_crc++;
    }
}

// Initialize the LCD controller
void GLCD_Init(bool rotate_180, sGLCD_Interface const *lcd_interface) {
    g_lcd_interface = lcd_interface;

    g_lcd_interface->rst(true);
    _delay_us(50);
    g_lcd_interface->rst(false);
    _delay_us(5);

    g_rotate_180 = rotate_180;

    g_lcd_interface->spi_cs(true);

    _glcd_write_cmd(GLCD_Cmd_InternalReset);
    _glcd_write_cmd(GLCD_Cmd_OnOff);
    _glcd_write_cmd(GLCD_Cmd_SetBias | 0x0);
    _glcd_write_cmd(GLCD_Cmd_SetAdcMode | rotate_180);
    _glcd_write_cmd(GLCD_Cmd_SetDispMode | false);
    _glcd_write_cmd(GLCD_Cmd_SetCOMOutputMode | (g_rotate_180 << 3));
    _glcd_write_cmd(GLCD_Cmd_SetAllPointsMode);
    _glcd_write_cmd(GLCD_Cmd_SetPowerControl | 0x7);
    _glcd_write_cmd(GLCD_Cmd_SetResistorRatio | 0x4);
    _glcd_write_cmd(GLCD_Cmd_SetVolume);
    _glcd_write_cmd(0x32);

    _glcd_write_cmd(GLCD_Cmd_OnOff | 0x1);

    g_lcd_interface->spi_cs(false);
}

