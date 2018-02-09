#ifndef MEMXOR_H_
#define MEMXOR_H_
#include <stdint.h>

void memxor(void* dest, const void* src, uint16_t n);

void memxor_ex(void* dest, const void* src1, const void* src2, uint16_t n);

#endif
