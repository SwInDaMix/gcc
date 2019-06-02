#include "sensors.h"
#include "utils.h"

static sSensors s_sensors;

void sensors_init() { }

sSensors const *sensors_get_current() { return &s_sensors; }
