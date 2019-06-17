#pragma once

#include <stdint.h>
#include "config.h"

#define HT1622_SCREEN_BUFFER_SIZE 32

typedef struct {
    uint8_t buffer[HT1622_SCREEN_BUFFER_SIZE];
} sHT1622Frame;

void ht1622_init();
void ht1622_deinit();
void ht1622_send_frame(sHT1622Frame const *frame);
