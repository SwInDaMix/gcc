/********************* (C) 2014 Eta Software House. ********************
 Author    : Sw
 File Name : System.cpp
 ***********************************************************************/

#include "config.h"
#include "eta_memman.h"
#include "eta_dbg.h"

void *operator new(size_t size) throw() { return malloc(size); }
void *operator new[](size_t size) throw() { return malloc(size); }
void operator delete(void *ptr) throw() { free(ptr); }
void operator delete[](void *ptr) throw() { free(ptr); }

extern "C" int32_t __aeabi_atexit(void *object, void (*destructor)(void *), void *dso_handle) { DBG_PutString("__aeabi_atexit\n"); return 0; }
extern "C" void __cxa_pure_virtual() { DBG_PutString("__cxa_pure_virtual\n"); while(true); }
