#pragma once
#include <inttypes.h>

extern size_t g_baseAddress;

// Only when working directly with one version of exe. Do not use anywhere other than testZone.cpp!
#define FIX_ADDR(addr) (addr - 0x400000 + g_baseAddress)

void injectFunc(size_t addr, size_t pfn);
size_t makeCall(size_t addr, size_t pfn);

template<typename T> size_t getThisCallAddr(T func) { return (size_t)(void*&)func; }

void makeNop(size_t addr, size_t size);
uint32_t writeDWORD(size_t addr, uint32_t val);
uint8_t writeBYTE(size_t addr, uint8_t val);
size_t setFnAddrInCallOpcode(size_t callPos, size_t pfn);
size_t getFnAddrInCallOpcode(size_t callPos);
