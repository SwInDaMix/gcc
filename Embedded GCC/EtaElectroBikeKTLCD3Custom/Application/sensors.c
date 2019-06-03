#include "sensors.h"
#include "periph.h"

static sSensors s_sensors;

sSensors const *sensors_get_current() {
    s_sensors.voltage = periph_get_adc_voltage();
    return &s_sensors;
}
