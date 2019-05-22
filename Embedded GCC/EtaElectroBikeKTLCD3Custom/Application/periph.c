#include <stddef.h>
#include "config.h"
#include "periph.h"
#include "main.h"

#define _gpio_init(name) GPIO_Init(name##__PORT, name##__PIN, name##__MODE)
#define _gpio_set_exti(port) EXTI_SetExtIntSensitivity(EXTI_PORT_GPIO##port, EXTI_IT_PORT##port##_SENS)
#define _gpio_read(name) GPIO_ReadInputPin(name##__PORT, name##__PIN)
#define _gpio_write(name, value) if(value) GPIO_WriteHigh(name##__PORT, name##__PIN); else GPIO_WriteLow(name##__PORT, name##__PIN)

#define _gpio_triger_EXTI_SENSITIVITY_RISE_FALL(name, callback) if((callback) && (_xor & name##__PINS)) { callback(_current & name##__PIN); }
#define _gpio_triger_EXTI_SENSITIVITY_FALL_ONLY(name, callback) if((callback) && (_xor & name##__PINS) && !(_current & name##__PIN)) { callback(); }

#define _periph_read_adc_channel(name) (*(uint8_t*)(uint16_t)((uint16_t)ADC1_BaseAddress + (uint8_t)(name##__CH << 1)))
#define _periph_read_adc_channel_10bits(name) ((((uint16_t)_periph_read_adc_channel(name)) << 2) | (*(uint8_t*)(uint16_t)((uint16_t)ADC1_BaseAddress + (uint8_t)((name##__CH << 1) + 1))))

static periph_lcd_timer_overflow_callback_t s_periph_lcd_timer_overflow_callback = 0;
static periph_uart_on_received_callback_t s_periph_uart_on_received_callback = 0;

static uint16_t s_periph_adc_counter = 0;
static uint16_t s_periph_adc_battery_voltage;

static volatile uint32_t s_periph_halfseconds;

void periph_init() {
    uint16_t _timer_start;

    //set clock at the max 16MHz
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

    // General purpose inputs/outputs
    _gpio_init(BUTTON_UP);
    _gpio_init(BUTTON_ONOFF);
    _gpio_init(BUTTON_DOWN);
    _gpio_init(ONOFF_POWER);
    _gpio_init(POWER_UP);
    _gpio_init(HT1622_CS);
    _gpio_init(HT1622_WRITE);
    _gpio_init(HT1622_READ);
    _gpio_init(HT1622_DATA);
    _gpio_init(HT1622_VDD);

    // Init GPIO to be used as ADC inputs
    _gpio_init(BATTERY_VOLTAGE);
    ADC1_DeInit();
    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_9, ADC1_PRESSEL_FCPU_D18, ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_LEFT, ADC1_SCHMITTTRIG_ALL, DISABLE);
    ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
    ADC1_Cmd(ENABLE);

    // Timer1 is used to create a PWM duty-cyle signal to control LCD backlight
    TIM1_DeInit();
    TIM1_TimeBaseInit(256, TIM1_COUNTERMODE_DOWN, 255, 0);
    TIM1_OC4Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE, 255, TIM1_OCPOLARITY_LOW, TIM1_OCIDLESTATE_RESET);
    TIM1_ARRPreloadConfig(ENABLE);
    TIM1_Cmd(ENABLE);
    TIM1_CtrlPWMOutputs(ENABLE);

    // Period measure timer (ticks every 0,512 msec, 1953,125 per sec)
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_8192, 0xFFFF);
    TIM2_Cmd(ENABLE);

    // Half seconds timer
    TIM3_DeInit();
    TIM3_TimeBaseInit(TIM3_PRESCALER_8, 1000);
    TIM3_ITConfig(TIM3_IT_UPDATE, ENABLE);
    TIM3_Cmd(ENABLE);

    // Init UART
    UART2_DeInit();
    UART2_Init((uint32_t)UART_BAUDRATE, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);
    UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
    UART2_Cmd(ENABLE);

    // Configure watchdog
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_4);
    IWDG_SetReload(1);
    IWDG_ReloadCounter();

    enableInterrupts();
    _timer_start = 0xFFF;
    while (_timer_start--);
    DBG("Periph system initialization done!\n");
}

void ADC1_IRQHandler() __interrupt(ADC1_IRQHANDLER) {
    static uint16_t _s_periph_adc_battery_voltage_acc;

    ADC1->CSR = ADC1_IT_EOCIE | (uint8_t)(ADC1_CHANNEL_9);

    _s_periph_adc_battery_voltage_acc = _s_periph_adc_battery_voltage_acc - s_periph_adc_battery_voltage + _periph_read_adc_channel_10bits(BATTERY_VOLTAGE);
    s_periph_adc_battery_voltage = _s_periph_adc_battery_voltage_acc >> 2;

    s_periph_adc_counter++;
}

uint16_t periph_get_adc_counter() { return s_periph_adc_counter; }
uint16_t periph_get_adc_battery_voltage() { return s_periph_adc_battery_voltage; }

ePeriphButton periph_get_buttons() {
    ePeriphButton _res = (ePeriphButton)0;

    if(_gpio_read(BUTTON_UP)) _res |= PeriphButton_Up;
    if(_gpio_read(BUTTON_ONOFF)) _res |= PeriphButton_OnOff;
    if(_gpio_read(BUTTON_DOWN)) _res |= PeriphButton_Down;

    return _res;
}

void periph_set_onoff_power(bool onoff_power) { _gpio_write(ONOFF_POWER, onoff_power); }
void periph_set_power_up(bool power_up) { _gpio_write(POWER_UP, power_up); }

void periph_set_backlight_pwm_duty_cycle(uint8_t duty_cycle) { TIM1_SetCompare4(UINT8_MAX - duty_cycle); }

void periph_set_ht1622_vdd(bool vdd) { _gpio_write(HT1622_VDD, vdd); }
void periph_set_ht1622_cs(bool cs) { _gpio_write(HT1622_CS, cs); }
void periph_set_ht1622_write(bool write) { _gpio_write(HT1622_WRITE, write); }
void periph_set_ht1622_read(bool read) { _gpio_write(HT1622_READ, read); }
void periph_set_ht1622_data(bool data) { _gpio_write(HT1622_DATA, data); }

void UART2_IRQHandler() __interrupt(UART2_IRQHANDLER) {
    uint8_t _byte;
    if (UART2_GetFlagStatus(UART2_FLAG_RXNE) == SET) {
        _byte = UART2_ReceiveData8();
        if (s_periph_uart_on_received_callback) s_periph_uart_on_received_callback(_byte);
    } else {
        if (UART2_GetFlagStatus(UART2_FLAG_OR_LHE) == SET) {
            UART2_ReceiveData8();  // -> clear!
        }
        if (UART2_GetFlagStatus(UART2_FLAG_FE) == SET) {
            UART2_ReceiveData8();  // -> clear!
        }
    }
}

void TIM3_UPD_OVF_BRK_IRQHandler() __interrupt(TIM3_UPD_OVF_BRK_IRQHANDLER) {
    static uint16_t _s_hs = 0;

    if(s_periph_lcd_timer_overflow_callback && !(s_periph_halfseconds & 1)) s_periph_lcd_timer_overflow_callback();
    _s_hs++;
    if(_s_hs >= 1000) { _s_hs = 0; s_periph_halfseconds++; }
    TIM3_ClearITPendingBit(TIM3_IT_UPDATE);
}

uint32_t periph_get_halfseconds() { uint32_t _halfseconds; disableInterrupts(); _halfseconds = s_periph_halfseconds; enableInterrupts(); return _halfseconds; }
uint16_t periph_get_timer() { return TIM2_GetCounter(); }

void putchar(unsigned char character) {
    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
    UART2_SendData8(character);
}
void periph_uart_set_on_received_callback(periph_uart_on_received_callback_t callback) { s_periph_uart_on_received_callback = callback; }
void periph_uart_putbyte(uint8_t byte) { putchar(byte); }

void periph_wdt_enable() { IWDG_Enable(); IWDG_ReloadCounter(); }
void periph_wdt_reset() { IWDG_ReloadCounter(); }