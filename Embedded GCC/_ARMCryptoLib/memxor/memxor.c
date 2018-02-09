#include <stdint.h>

#include "memxor.h"

void memxor(void* dest, const void* src, uint16_t n){
  while(n--){
    *((uint8_t*)dest) ^= *((uint8_t*)src);
    dest = (uint8_t*)dest +1;
    src  = (uint8_t*)src  +1;
  }
}

void memxor_ex(void* dest, const void* src1, const void* src2, uint16_t n){
  while(n--){
    *((uint8_t*)dest) = *((uint8_t*)src1) ^ *((uint8_t*)src2);
    dest = (uint8_t*)dest +1;
    src1 = (uint8_t*)src1 +1;
    src2 = (uint8_t*)src2 +1;
  }
}

