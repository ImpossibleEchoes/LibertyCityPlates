#include <Windows.h>
#include <stdio.h>

#include "addrs.h"

size_t g_baseAddress = (size_t)GetModuleHandleA(nullptr);

DWORD g_dwOldProtect;

bool setProtect(size_t addr, size_t size, uint32_t newProtect) {
	size = (size + 0xfff) & ~0xfff;
	return VirtualProtect((void*)addr, size, newProtect, &g_dwOldProtect);
}

void injectFunc(size_t addr, size_t pfn) {

	uint8_t* patch = (uint8_t*)addr + 1;

	auto retVal = (*(uint32_t*)patch + (addr + 5));
	setProtect(addr, 5, PAGE_EXECUTE_READWRITE);
	*(patch - 1) = 0xE9;
	*(uint32_t*)patch = (pfn - (addr + 5));
	setProtect(addr, 5, g_dwOldProtect);

}

size_t makeCall(size_t addr, size_t pfn) {

	uint8_t* patch = (uint8_t*)addr + 1;

	auto retVal = (*(uint32_t*)patch + (addr + 5));
	setProtect(addr, 5, PAGE_EXECUTE_READWRITE);
	*(patch - 1) = 0xe8;
	*(uint32_t*)patch = (pfn - (addr + 5));
	setProtect(addr, 5, g_dwOldProtect);

	return retVal;
}

uint32_t writeDWORD(size_t addr, uint32_t val) {

	//uint32_t oldVal = *(uint32_t*)addr;
	//*(uint32_t*)addr = val;

	//return oldVal;

	setProtect(addr, sizeof val, PAGE_EXECUTE_READWRITE);

	uint32_t oldVal = *(uint32_t*)addr;
	*(uint32_t*)addr = val;

	setProtect(addr, sizeof val, g_dwOldProtect);

	return oldVal;
}

void makeNop(size_t addr, size_t size) {

	//uint32_t oldVal = *(uint32_t*)addr;
	//*(uint32_t*)addr = val;

	//return oldVal;

	setProtect(addr, size, PAGE_EXECUTE_READWRITE);

	memset((void*)addr, 0x90, size);

	setProtect(addr, size, g_dwOldProtect);

}

uint8_t writeBYTE(size_t addr, uint8_t val) {

	//uint8_t oldVal = *(uint8_t*)addr;
	//*(uint8_t*)addr = val;

	//return oldVal;

	setProtect(addr, sizeof val, PAGE_EXECUTE_READWRITE);

	uint8_t oldVal = *(uint8_t*)addr;
	*(uint8_t*)addr = val;

	setProtect(addr, sizeof val, g_dwOldProtect);

	return oldVal;
}

size_t setFnAddrInCallOpcode(size_t callPos, size_t pfn) {

	uint8_t* patch = (uint8_t*)callPos + 1;

	auto retVal = (*(uint32_t*)patch + (callPos + 5));
	setProtect(callPos, 5, PAGE_EXECUTE_READWRITE);
	*(uint32_t*)patch = (pfn - (callPos + 5));
	setProtect(callPos, 5, g_dwOldProtect);

	return retVal;

}

size_t getFnAddrInCallOpcode(size_t callPos) {

	uint8_t* patch = (uint8_t*)callPos + 1;

	auto retVal = (*(uint32_t*)patch + (callPos + 5));

	return retVal;

}
