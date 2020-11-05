/********************* (C) 2020 Eta Software House. ********************
 Author    : Sw
 File Name : sensors.c
 ***********************************************************************/

#include <eta_flash.h>
#include <melody.h>
#include "periphs.h"
#include "config.h"
#include "stdperiph.h"
#include "eta_st7565.h"
#include "eta_inputs.h"
#include "eta_smart_backlight.h"
#include "eta_crc.h"

/// ***************************************************************************
/// Graphic LCD display periph
/// ***************************************************************************

static bit g_lcd_is_data;
static void _glcd_rst(bool state) {
    GPIO_WriteBit(GLCD_RST_PORT, (1 << GLCD_RST_PIN), state ? Bit_RESET : Bit_SET);
}
static void _glcd_a0(bool state) {
    if (g_lcd_is_data != state) {
        g_lcd_is_data = state;
        while(SPI_I2S_GetFlagStatus(GLCD_SPI_PERIPH, SPI_I2S_FLAG_BSY) == SET);
    }
    GPIO_WriteBit(GLCD_A0_PORT, (1 << GLCD_A0_PIN), state ? Bit_SET : Bit_RESET);
}
static void _glcd_spi_cs(bool state) {
    if (state) {
        GPIO_ResetBits(GLCD_CS_PORT, (1 << GLCD_CS_PIN));
    } else {
        while(SPI_I2S_GetFlagStatus(GLCD_SPI_PERIPH, SPI_I2S_FLAG_BSY) == SET);
        GPIO_SetBits(GLCD_CS_PORT, (1 << GLCD_CS_PIN));
    }
}
static void _glcd_spi_write(uint8_t data) {
    while(SPI_I2S_GetFlagStatus(GLCD_SPI_PERIPH, SPI_I2S_FLAG_TXE) == RESET);
    SPI_SendData8(GLCD_SPI_PERIPH, data);
}
static sGLCD_Interface const g_glcd_interface = {
    .rst = _glcd_rst,
    .a0 = _glcd_a0,
    .spi_cs = _glcd_spi_cs,
    .spi_write = _glcd_spi_write
};

/// ***************************************************************************
/// SPI Flash periph
/// ***************************************************************************

static bit g_spi_flash_is_reading;
static void _flash_spi_cs(bool state) {
    if (state) {
        GPIO_ResetBits(SPIFLASH_CS_PORT, (1 << SPIFLASH_CS_PIN));
        g_spi_flash_is_reading = false;
    } else {
        while(SPI_I2S_GetFlagStatus(SPIFLASH_SPI_PERIPH, SPI_I2S_FLAG_BSY) == SET);
        GPIO_SetBits(SPIFLASH_CS_PORT, (1 << SPIFLASH_CS_PIN));
    }
}
static uint8_t _flash_spi_read(bool is_need_more) {
    if (!g_spi_flash_is_reading) {
        g_spi_flash_is_reading = true;
        while(SPI_I2S_GetFlagStatus(SPIFLASH_SPI_PERIPH, SPI_I2S_FLAG_BSY) == SET || SPI_I2S_GetFlagStatus(SPIFLASH_SPI_PERIPH, SPI_I2S_FLAG_RXNE) == SET) SPI_ReceiveData8(SPIFLASH_SPI_PERIPH);
        SPI_SendData8(SPIFLASH_SPI_PERIPH, 0);
    }
    if (is_need_more) {
        while(SPI_I2S_GetFlagStatus(GLCD_SPI_PERIPH, SPI_I2S_FLAG_TXE) == RESET);
        SPI_SendData8(GLCD_SPI_PERIPH, 0);
    }
    while(SPI_I2S_GetFlagStatus(SPIFLASH_SPI_PERIPH, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_ReceiveData8(SPIFLASH_SPI_PERIPH);
}
static void _flash_spi_write(uint8_t data) {
    g_spi_flash_is_reading = false;
    while(SPI_I2S_GetFlagStatus(GLCD_SPI_PERIPH, SPI_I2S_FLAG_TXE) == RESET);
    SPI_SendData8(GLCD_SPI_PERIPH, data);
}
static sSpiFlash_Interface const g_spi_flash_interface = {
    .spi_cs = _flash_spi_cs,
    .spi_read = _flash_spi_read,
    .spi_write = _flash_spi_write
};

/// ***************************************************************************
/// Buzzer periph
/// ***************************************************************************

#define BUZZER_PWM(freq) ((uint32_t)(F_CPU / freq * 512))

static const uint32_t g_buzzer_frequencies[] = {BUZZER_PWM(16744.0), BUZZER_PWM(17739.2), BUZZER_PWM(18793.6), BUZZER_PWM(19912.0), BUZZER_PWM(21096.0), BUZZER_PWM(22350.4), BUZZER_PWM(23680.0), BUZZER_PWM(25088.0), BUZZER_PWM(26579.2), BUZZER_PWM(28160.0), BUZZER_PWM(29833.6), BUZZER_PWM(31608.0) };
static void _buzzer_irq_safe(d_buzzer_block block) {
    NVIC_DisableIRQ(TIM_LCD_BL_IRQn);
    block();
    NVIC_EnableIRQ(TIM_LCD_BL_IRQn);
}
static void _buzzer_set_silence() {
    TIM_SetCompare1(BUZZER_TIM, 0);
}
static void _buzzer_set_wave(eBuzzerNote note, eBuzzerOctave octave) {
    if (note == BuzzerNote_Silence) _buzzer_set_silence();
    else {
        uint32_t _f_period = g_buzzer_frequencies[note - 1] >> octave;
        uint16_t _prescaler = (uint16_t)(_f_period >> 16); _f_period /= (_prescaler + 1);
        TIM_Cmd(BUZZER_TIM, DISABLE);
        TIM_PrescalerConfig(BUZZER_TIM, _prescaler, TIM_PSCReloadMode_Update);
        TIM_SetAutoreload(BUZZER_TIM, _f_period);
        TIM_SetCompare1(BUZZER_TIM, _f_period >> 1);
        TIM_Cmd(BUZZER_TIM, ENABLE);
    }
}
sBuzzer_Interface const g_buzzer_interface = {
    .irq_safe = _buzzer_irq_safe,
    .set_wave = _buzzer_set_wave,
    .set_silence = _buzzer_set_silence
};

/// ***************************************************************************
/// LCD backlight control periph
/// ***************************************************************************

#define SMARTBACKLIGHT_CALC(x) ((x) * (x))

static void _smart_backlight_irq_safe(d_smart_backlight_block block) {
    NVIC_DisableIRQ(TIM_LCD_BL_IRQn);
    block();
    NVIC_EnableIRQ(TIM_LCD_BL_IRQn);
}
static void _smart_backlight_set_brightness(sSmartBacklightInstance *smart_backlight_instance) {
    uint8_t backlight = smart_backlight_instance->backlight;
    TIM_SetCompare1(TIM_LCD_BL, TIM_LCD_BL_PERIOD + 1 - ((uint64_t)(TIM_LCD_BL_PERIOD + 1) * SMARTBACKLIGHT_CALC(backlight) / SMARTBACKLIGHT_CALC(UINT8_MAX)));
}
sSmartBacklightInterface const g_smart_backlight_interface = {
    .irq_safe = _smart_backlight_irq_safe,
    .set_brightness = _smart_backlight_set_brightness
};
sSmartBacklightInstance g_smart_backlight_instance_lcd;
static void LCDBacklight_Periodic() { SmartBacklight_Periodic(&g_smart_backlight_instance_lcd); }
static void LCDBacklight_WakeUp() { SmartBacklight_WakeUp(&g_smart_backlight_instance_lcd); }

/// ***************************************************************************
/// Inputs periph
/// ***************************************************************************

static ePeriphInput g_periph_input;
static sInputInstance_Buttons g_instance_buttons_main;
static sInputInstance_Encoder g_instance_encoder_main;
static bool Inputs_Periodic() {
    // **************
    // Buttons Update
    // **************
    ePeriphButton _btn_periph = PeriphButton_None;
    if (GPIO_ReadInputDataBit(BTN_ESCAPE_PORT, (1 << BTN_ESCAPE_PIN)) == Bit_RESET) _btn_periph |= PeriphButton_Escape;
    if (GPIO_ReadInputDataBit(BTN_ENTER_PORT, (1 << BTN_ENTER_PIN)) == Bit_RESET) _btn_periph |= PeriphButton_Enter;
    uint32_t _btn_pressed = Input_ButtonsProcess(_btn_periph, PeriphButton__Count, &g_instance_buttons_main);
    g_periph_input |= (ePeriphInput)_btn_pressed;

    // **************
    // Encoder Update
    // **************
    bool _enc_a = GPIO_ReadInputDataBit(BTN_ENCODER_A_PORT, (1 << BTN_ENCODER_A_PIN)) == Bit_RESET;
    bool _enc_b = GPIO_ReadInputDataBit(BTN_ENCODER_B_PORT, (1 << BTN_ENCODER_B_PIN)) == Bit_RESET;
    eInput_EncoderAction _enc_action = Input_EncoderProcess(_enc_a, _enc_b, &g_instance_encoder_main);
    if (_enc_action == EncoderAction_Forward) g_periph_input |= PeriphInput_Forward;
    else if (_enc_action == EncoderAction_Backward) g_periph_input |= PeriphInput_Backward;

    return _btn_pressed || _enc_action;
}

/// ***************************************************************************
/// Periodic processing
/// ***************************************************************************

static uint32_t volatile g_global_ticks;
static uint32_t volatile g_global_ms10ticks;
void Periph_Periodic() {
    g_global_ticks++;

    static uint32_t _ticks_10ms_prev;
    if ((g_global_ticks - _ticks_10ms_prev) > (PERIODIC_TICKS_PER_SECOND / 100)) {
        g_global_ms10ticks++;
        _ticks_10ms_prev += (PERIODIC_TICKS_PER_SECOND / 100);
    }

    if (Inputs_Periodic()) LCDBacklight_WakeUp();
    Buzzer_Periodic();
    LCDBacklight_Periodic();
}

void Periph_Init() {
    GLCD_Init(false, &g_glcd_interface);
    SpiFlash_Init(&g_spi_flash_interface);
    Buzzer_Init(&g_buzzer_interface);
    SmartBacklight_Init(&g_smart_backlight_interface);
}

ePeriphInput Periph_GetInput() {
    ePeriphInput _periph_input = g_periph_input;
    g_periph_input = PeriphInput_None;
    return _periph_input;
}

uint32_t Periph_GetGlobalTicks() {
    return g_global_ticks;
}

uint32_t Periph_GetGlobalMs10Ticks() {
    return g_global_ms10ticks;
}

uint32_t Periph_Random() {
    uint32_t _rnd[] = { TIM_GetCounter(TIM_LCD_BL), g_global_ticks };
    return crc32_fast(UINT32_MAX, _rnd, sizeof(_rnd));
}

void Periph_LCDBacklight_SetParams(uint8_t backlight_wakeup, uint8_t backlight_background, uint8_t backlight_standby, uint8_t timeout) { SmartBacklight_SetParams(&g_smart_backlight_instance_lcd, backlight_wakeup, backlight_background, backlight_standby, timeout); }

void Periph_LCDBacklight_SetBackgroundMode(bool is_background) { SmartBacklight_SetBackgroundMode(&g_smart_backlight_instance_lcd, is_background); }
