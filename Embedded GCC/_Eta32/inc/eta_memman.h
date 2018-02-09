/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : eta_memman.h
***********************************************************************/

#pragma once

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initializes memory heap
void memman_initialize(void *heap_start, uint16_t heap_size);
// Returns amount of free bytes available for allocation
uint16_t memman_get_free();
// Returns amount of minimum free bytes ever remaining since last system boot
uint16_t memman_get_minimum_free();
// Returns amount of maximum busy bytes ever allocated since last system boot
uint16_t memman_get_maximum_busy();
// Returns amount of maximum free bytes that can be allocated as single block
uint16_t memman_get_maximum_free_block();

void *malloc(size_t size);
void free(void *ptr);

#ifdef __cplusplus
}
#endif
