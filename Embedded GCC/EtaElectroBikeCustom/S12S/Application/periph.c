#include <string.h>
#include <stddef.h>
#include "config.h"
#include "periph.h"
#include "main.h"
#include "bldc.h"

#define _gpio_init(name) GPIO_Init(name##__PORT, name##__PINS, name##__MODE)
#define _gpio_set_exti(port) EXTI_SetExtIntSensitivity(EXTI_PORT_GPIO##port, EXTI_IT_PORT##port##_SENS)
#define _gpio_read(name) GPIO_ReadInputPin(name##__PORT, name##__PINS)
#define _gpio_write(name, value) if(value) GPIO_WriteHigh(name##__PORT, name##__PINS); else GPIO_WriteLow(name##__PORT, name##__PINS)

#define _gpio_triger_EXTI_SENSITIVITY_RISE_FALL(name, callback) if((callback) && (_xor & name##__PINS)) { callback(_current & name##__PINS); }
#define _gpio_triger_EXTI_SENSITIVITY_FALL_ONLY(name, callback) if((callback) && (_xor & name##__PINS) && !(_current & name##__PINS)) { callback(); }

#define _periph_read_adc_channel(name) (*(uint8_t*)(uint16_t)((uint16_t)ADC1_BaseAddress + (uint8_t)(name##__CH << 1)))
#define _periph_read_adc_channel_10bits(name) ((((uint16_t)_periph_read_adc_channel(name)) << 2) | (*(uint8_t*)(uint16_t)((uint16_t)ADC1_BaseAddress + (uint8_t)((name##__CH << 1) + 1))))

static periph_bldc_pwm_overflow_callback_t s_periph_bldc_pwm_overflow_callback = 0;
static periph_overcurrent_callback_t s_periph_overcurrent_callback = 0;
static periph_pas_callback_t s_periph_pas_callback = 0;
static periph_speed_callback_t s_periph_speed_callback = 0;
static periph_brake_callback_t s_periph_brake_callback = 0;
static periph_uart_on_received_callback_t s_periph_uart_on_received_callback = 0;
static periph_uart_on_transmit_callback_t s_periph_uart_on_transmit_callback = 0;

static uint16_t s_periph_adc_counter = 0;
static uint16_t s_periph_adc_phase_b_current_calibration;
static uint16_t s_periph_adc_phase_b_current;
static uint16_t s_periph_adc_motor_current_calibration;
static uint16_t s_periph_adc_motor_current;
static uint16_t s_periph_adc_motor_current_filtered;
static uint16_t s_periph_adc_motor_temperature;
static uint16_t s_periph_adc_battery_voltage;
static uint8_t s_periph_adc_thumb_throttle;
static uint8_t s_periph_adc_thumb_brake;

void periph_init() {
    uint16_t _timer_start;
    uint16_t _counter_adc_prev = 0, _counter_adc, _counter_adc_calibration = 0;
    uint32_t _adc_phase_b_current_calibration = 0, _adc_motor_current_calibration = 0, _adc_motor_filtered_current_calibration = 0;

    //set clock at the max 16MHz
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);

    // Init timer 1 and hall sensors inputs for BLDC
    _gpio_init(HALL_SENSORS);
    TIM1_DeInit();
    TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, (1 << BLDC_PWM_FULL_DUTY_BITS) - 1, 0);
    TIM1_OC1Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_ENABLE, 0, TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_SET);
    TIM1_OC2Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_ENABLE, 0, TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_SET);
    TIM1_OC3Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE, TIM1_OUTPUTNSTATE_ENABLE, 0, TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_SET);
    TIM1_OC1PreloadConfig(ENABLE);
    TIM1_OC2PreloadConfig(ENABLE);
    TIM1_OC3PreloadConfig(ENABLE);
    TIM1_BDTRConfig(TIM1_OSSISTATE_ENABLE, TIM1_LOCKLEVEL_OFF, 16, /* DTG = 0; dead time in 62.5 ns steps; 1us/62.5ns = 16 */ TIM1_BREAK_DISABLE, TIM1_BREAKPOLARITY_LOW, TIM1_AUTOMATICOUTPUT_DISABLE);
    TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
    TIM1_Cmd(ENABLE);
    TIM1_CtrlPWMOutputs(DISABLE);

    // Init GPIO to be used as ADC inputs
    _gpio_init(PHASE_B_CURRENT);
    _gpio_init(MOTOR_CURRENT);
    _gpio_init(MOTOR_TEMPERATURE);
    _gpio_init(BATTERY_VOLTAGE);
    _gpio_init(THUMB_THROTTLE);
    _gpio_init(THUMB_BRAKE);
    ADC1_DeInit();
    ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_9, ADC1_PRESSEL_FCPU_D18, ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_LEFT, ADC1_SCHMITTTRIG_ALL, DISABLE);
    ADC1_ScanModeCmd(ENABLE);
    ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
    ADC1_Cmd(ENABLE);

    // General purpose inputs/outputs
    //_gpio_init(MOTOR_OVERCURRENT);
    _gpio_init(BRAKE);
    _gpio_init(PAS);
    _gpio_init(SPEED);
    _gpio_init(HEAD_LIGHT_FEEDBACK);
    _gpio_init(HEAD_LIGHT);

    // configure EXTI for ports
    _gpio_set_exti(A);
    _gpio_set_exti(C);
    _gpio_set_exti(D);

    // Period measure timer (ticks every
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_8192, 0xFFFF);
    TIM2_Cmd(ENABLE);

    // Tail light PWM timer
    TIM3_DeInit();
    TIM3_TimeBaseInit(TIM3_PRESCALER_16, 255);
    TIM3_OC1Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE, 0, TIM3_OCPOLARITY_HIGH);
    TIM3_OC1PreloadConfig(ENABLE);
    TIM3_Cmd(ENABLE);

    // Init UART
    UART2_DeInit();
    UART2_Init((uint32_t)UART_BAUDRATE, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);
    UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
    UART2_Cmd(ENABLE);

    // Configure watchdog
    IWDG_Enable();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_4);
    IWDG_SetReload(1);
    IWDG_ReloadCounter();

    enableInterrupts();
    _timer_start = 0xFFF;
    while (_timer_start--);
    DBG("Periph system initialization done!\n");

    _timer_start = periph_get_timer();
    while ((periph_get_timer() - _timer_start) < TIMER_TP_MS(PERIPH_ADC_STABILIZATION_PERIOD_MS));
    DBG("Periph ADC stabilization done!\n");

    //uint32_t _adc_phase_b_current_calibration = 0, _adc_motor_current_calibration = 0, _adc_motor_filtered_calibration = 0, _adc_calibration_cnt = 0;
    _timer_start = periph_get_timer();
    while ((periph_get_timer() - _timer_start) < TIMER_TP_MS(PERIPH_ADC_CALIBRATION_READS_PERIOD_MS)) {
        _counter_adc = periph_get_adc_counter();
        if (_counter_adc_prev != _counter_adc) {
            _counter_adc_prev = _counter_adc;
            _adc_phase_b_current_calibration += periph_get_adc_phase_b_current();
            _adc_motor_current_calibration += periph_get_adc_motor_current();
            _counter_adc_calibration++;
        }
    }
    s_periph_adc_phase_b_current_calibration = (uint16_t)(_adc_phase_b_current_calibration / _counter_adc_calibration);
    s_periph_adc_motor_current_calibration = (uint16_t)(_adc_motor_current_calibration / _counter_adc_calibration);
    DBGF("Periph ADC calibration done with (%d) cycles\n", _counter_adc_calibration);
    DBGF(" Phase B current: (%d)\n", s_periph_adc_phase_b_current_calibration);
    DBGF(" Motor current: (%d)\n", s_periph_adc_motor_current_calibration);
}

void TIM1_UPD_OVF_TRG_BRK_IRQHandler() __interrupt(TIM1_UPD_OVF_TRG_BRK_IRQHANDLER) {
    TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
    if (s_periph_bldc_pwm_overflow_callback) s_periph_bldc_pwm_overflow_callback();
}

void ADC1_IRQHandler() __interrupt(ADC1_IRQHANDLER) {
    static uint16_t _s_periph_adc_motor_temperature_acc;
    static uint16_t _s_periph_adc_motor_filtered_current_acc;
    static uint16_t _s_periph_adc_battery_voltage_acc;
    static uint16_t _s_periph_adc_thumb_throttle_acc;
    static uint16_t _s_periph_adc_thumb_brake_acc;

    ADC1->CSR = ADC1_IT_EOCIE | (uint8_t)(ADC1_CHANNEL_9);

    s_periph_adc_phase_b_current = _periph_read_adc_channel_10bits(PHASE_B_CURRENT);

    s_periph_adc_motor_current = _periph_read_adc_channel_10bits(MOTOR_CURRENT);

    _s_periph_adc_motor_filtered_current_acc = _s_periph_adc_motor_filtered_current_acc - s_periph_adc_motor_current_filtered + s_periph_adc_motor_current;
    s_periph_adc_motor_current_filtered = _s_periph_adc_motor_filtered_current_acc >> 6;

    _s_periph_adc_motor_temperature_acc = _s_periph_adc_motor_temperature_acc - s_periph_adc_motor_temperature + _periph_read_adc_channel_10bits(MOTOR_TEMPERATURE);
    s_periph_adc_motor_temperature = _s_periph_adc_motor_temperature_acc >> 6;

    _s_periph_adc_battery_voltage_acc = _s_periph_adc_battery_voltage_acc - s_periph_adc_battery_voltage + _periph_read_adc_channel_10bits(BATTERY_VOLTAGE);
    s_periph_adc_battery_voltage = _s_periph_adc_battery_voltage_acc >> 2;

    _s_periph_adc_thumb_throttle_acc = _s_periph_adc_thumb_throttle_acc - s_periph_adc_thumb_throttle + _periph_read_adc_channel(THUMB_THROTTLE);
    s_periph_adc_thumb_throttle = (uint8_t)(_s_periph_adc_thumb_throttle_acc >> 2);

    _s_periph_adc_thumb_brake_acc = _s_periph_adc_thumb_brake_acc - s_periph_adc_thumb_brake + _periph_read_adc_channel(THUMB_BRAKE);
    s_periph_adc_thumb_brake = (uint8_t)(_s_periph_adc_thumb_brake_acc >> 2);

    s_periph_adc_counter++;
}

void EXTI_PORTA_IRQHandler(void) __interrupt(EXTI_PORTA_IRQHANDLER) {
    static uint8_t _s_prev;
    uint8_t _current = GPIO_ReadInputData(GPIOA);
    uint8_t _xor = _s_prev ^_current;

    _gpio_triger_EXTI_SENSITIVITY_RISE_FALL(BRAKE, s_periph_brake_callback);

    _s_prev = _current;
}

void EXTI_PORTC_IRQHandler(void) __interrupt(EXTI_PORTC_IRQHANDLER) {
    static uint8_t _s_prev;
    uint8_t _current = GPIO_ReadInputData(GPIOC);
    uint8_t _xor = _s_prev ^_current;

    _gpio_triger_EXTI_SENSITIVITY_FALL_ONLY(SPEED, s_periph_speed_callback);

    _s_prev = _current;
}

void EXTI_PORTD_IRQHandler(void) __interrupt(EXTI_PORTD_IRQHANDLER) {
    static uint8_t _s_prev;
    uint8_t _current = GPIO_ReadInputData(GPIOD);
    uint8_t _xor = _s_prev ^_current;

    _gpio_triger_EXTI_SENSITIVITY_FALL_ONLY(MOTOR_OVERCURRENT, s_periph_overcurrent_callback);
    _gpio_triger_EXTI_SENSITIVITY_FALL_ONLY(PAS, s_periph_pas_callback);

    _s_prev = _current;
}

void UART2_IRQHandler(void) __interrupt(UART2_IRQHANDLER) {
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

void periph_atom_start() { disableInterrupts(); }
void periph_atom_end() { enableInterrupts(); }

void periph_set_bldc_pwm_overflow_callback(periph_bldc_pwm_overflow_callback_t callback) { s_periph_bldc_pwm_overflow_callback = callback; }
uint8_t periph_get_bldc_hall_sensors() { return _gpio_read(HALL_SENSORS); }
void periph_set_bldc_pwm_duty_cucles(uint16_t dc_a, uint16_t dc_b, uint16_t dc_c) {
    TIM1_SetCompare1(dc_a);
    TIM1_SetCompare2(dc_b);
    TIM1_SetCompare3(dc_c);
}
void periph_set_bldc_pwm_outputs(bool is_enabled) {
    static bool _s_is_enabled;
    if (_s_is_enabled != is_enabled) {
        _s_is_enabled = is_enabled;
        TIM1_CtrlPWMOutputs(is_enabled ? ENABLE : DISABLE);
    }
}

uint16_t periph_get_timer() { return TIM2_GetCounter(); }

uint16_t periph_get_adc_counter() { return s_periph_adc_counter; }
uint16_t periph_get_adc_phase_b_current_calibration() { return s_periph_adc_phase_b_current_calibration; }
uint16_t periph_get_adc_phase_b_current() { return s_periph_adc_phase_b_current; }
uint16_t periph_get_adc_motor_current_calibration() { return s_periph_adc_motor_current_calibration; }
uint16_t periph_get_adc_motor_current() { return s_periph_adc_motor_current; }
uint16_t periph_get_adc_motor_current_filtered() { return s_periph_adc_motor_current_filtered; }
uint16_t periph_get_adc_motor_temperature() { return s_periph_adc_motor_temperature; }
uint16_t periph_get_adc_battery_voltage() { return s_periph_adc_battery_voltage; }
uint8_t periph_get_adc_thumb_throttle() { return s_periph_adc_thumb_throttle; }
uint8_t periph_get_adc_thumb_break() { return s_periph_adc_thumb_brake; }

void periph_set_overcurrent_callback(periph_overcurrent_callback_t callback) { s_periph_overcurrent_callback = callback; }
void periph_set_pas_callback(periph_pas_callback_t callback) { s_periph_pas_callback = callback; }
void periph_set_speed_callback(periph_speed_callback_t callback) { s_periph_speed_callback = callback; }
void periph_set_brake_callback(periph_brake_callback_t callback) { s_periph_brake_callback = callback; }

bool periph_get_is_braking() { return !_gpio_read(BRAKE); }
bool periph_get_is_headlight_on() { return _gpio_read(HEAD_LIGHT_FEEDBACK); }

void periph_set_headlight(bool is_enabled) { _gpio_write(HEAD_LIGHT, is_enabled); }
void periph_set_taillight_pwm_duty_cycle(uint8_t duty_cycle) { TIM3_SetCompare1(duty_cycle); }

void putchar(unsigned char character) {
    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);
    UART2_SendData8(character);
}
void periph_uart_set_on_received_callback(periph_uart_on_received_callback_t callback) { s_periph_uart_on_received_callback = callback; }
void periph_uart_set_on_transmit_callback(periph_uart_on_transmit_callback_t callback) { s_periph_uart_on_transmit_callback = callback; UART2_ITConfig(UART2_IT_TXE, ENABLE); }
void periph_uart_putbyte(uint8_t byte) { putchar(byte); }

void periph_eeprom_read(void *dst, uint16_t offset, size_t size) {
    memcpy(dst, (void *)(FLASH_DATA_START_PHYSICAL_ADDRESS + offset), size);
}
void periph_eeprom_write(void const *src, uint16_t offset, size_t size){
    uint8_t _i;
    uint32_t *_src = (uint32_t *)src;
    uint32_t *_dst = (uint32_t *)(FLASH_DATA_START_PHYSICAL_ADDRESS + offset);

    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    while(!FLASH_GetFlagStatus(FLASH_FLAG_DUL));
    for (_i = 0; _i < ((size + 3) >> 2); _i++) {
        periph_wdt_reset();
        FLASH_ProgramWord((uint32_t)(_dst++), *_src++);
        while(!FLASH_GetFlagStatus(FLASH_FLAG_EOP));
    }
    FLASH_Lock(FLASH_MEMTYPE_DATA);

}

void periph_wdt_reset() { IWDG_ReloadCounter(); }