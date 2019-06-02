#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "periph.h"
#include "ht1622.h"
#include "display.h"
#include "network.h"
#include "sensors.h"
#include "settings.h"
#include "logic.h"
#include "buttons.h"
#include "utils.h"

int main(void) {
    periph_init();
    ht1622_init();
    settings_init();
    network_init();
    logic_init();

    periph_wdt_enable();

    while(disp_cycle(DispState_AnimateWithDigits) == DispState_AnimateWithDigits) {
        periph_wdt_reset();
        periph_set_onoff_power(periph_get_buttons() & PeriphButton_OnOff);
    }

    periph_set_onoff_power(true); // TODO: remove
    while(true) {
        periph_wdt_reset();

        network_cycle();
        logic_cycle(sensors_get_current(), settings_get_current());
    }
}
