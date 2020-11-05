/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : eta_st7565.h
 ***********************************************************************/

#pragma once

#include "config.h"

#define GLCD_ROWS 8
#define GLCD_WIDTH 128
#define GLCD_HEIGHT (GLCD_ROWS << 3)

// Commands
typedef enum {
    GLCD_Cmd_OnOff = 0xAE,              // .......x; x = 0 - off, 1 - on
    GLCD_Cmd_SetDispStartLine = 0x40,   // ..xxxxxx; x from 0 to 63 = display start line
    GLCD_Cmd_SetPage = 0xB0,            // ....xxxx; x from 0 to 15 = page address
    GLCD_Cmd_SetColumnUpper = 0x10,     // ....xxxx; x from 0 to 15 = most significant 4 bits of column
    GLCD_Cmd_SetColumnLower = 0x00,     // ....xxxx; x from 0 to 15 = least significant 4 bits of column
    GLCD_Cmd_SetAdcMode = 0xA0,         // .......x; x = 0 - normal, 1 - reversed
    GLCD_Cmd_SetDispMode = 0xA6,        // .......x; x = 0 - normal, 1 - reversed
    GLCD_Cmd_SetAllPointsMode = 0xA4,   // .......x; x = 0 - normal, 1 - all point are on
    GLCD_Cmd_SetBias = 0xA2,            // .......x; x = 0 - 1/9 bias, 1 - 1/7 bias
    GLCD_Cmd_InternalReset = 0xE2,      // ........;
    GLCD_Cmd_SetCOMOutputMode = 0xC0,   // ....x...; x = 0 - normal, 1 - reversed
    GLCD_Cmd_SetPowerControl = 0x28,    // .....xyz; x - booster circuit on/off; y - voltage regulator circuit on/off; z - voltage follower circuit on/off
    GLCD_Cmd_SetResistorRatio = 0x20,   // .....xxx; x from 0 to 7 = resistor ratio
    GLCD_Cmd_SetVolume = 0x81,          // ........; second command byte is ..xxxxxx; x from 0 to 63 = electronic volume
    GLCD_Cmd_SetStatic = 0xAC,          // .......x; x = 0 - off, 1 on; second command byte is .......y; y = 0 - normal; 1 - flashing
    GLCD_Cmd_SetBooster = 0xF8,         // ........; second command byte is ......xx; x = 0 - 2x|3x|4x, 1 - 5x, 3 - 6x
    GLCD_Cmd_Nop = 0xE3                 // ........;
} eGLCD_Cmd;

typedef void (*d_glcd_rst)(bool state);
typedef void (*d_glcd_a0)(bool state);
typedef void (*d_glcd_spi_cs)(bool state);
typedef void (*d_glcd_spi_send)(uint8_t byte);

typedef struct {
    d_glcd_rst rst;
    d_glcd_a0 a0;
    d_glcd_spi_cs spi_cs;
    d_glcd_spi_send spi_write;
} sGLCD_Interface;

// Get screen area
uint8_t *GLCD_GetScreen();
// Flushes screen area to display
void GLCD_Flush();
// Initialize the LCD controller
void GLCD_Init(bool rotate_180, sGLCD_Interface const *lcd_interface);

