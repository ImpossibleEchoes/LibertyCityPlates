#include "helpers.h"
#include "addrs.h"


float (*g_pfnGetVehRpm)(CVehicle*) = nullptr;
uint32_t(*g_pfnGetVehNumWheels)(CVehicle*) = nullptr;
CWheel* (*g_pfnGetVehWheels)(CVehicle*) = nullptr;
float(*g_pfnGetVehTemp)(CVehicle*) = nullptr;
CHandlingVehicle* (*g_pfnGetHandlingData)(CVehicle*) = nullptr;
char(*g_pfnGetVehFlags1_0)(CVehicle*) = nullptr;
float(*g_pfnGetVehThrottle)(CVehicle*) = nullptr;
uint32_t(*g_pfnGetVehSpecialType)(CVehicle*) = nullptr;
uint32_t(*g_pfnGetVehMainType)(CVehicle*) = nullptr;
float(*g_pfnGetVehGasPedal)(CVehicle*) = nullptr;
float(*g_pfnGetVehBrakePedal)(CVehicle*) = nullptr;
uint32_t(*g_pfnGetVeh_FE0)(CVehicle*) = nullptr;
fragInst*(*g_pfnGetVehFragInst)(CVehicle*) = nullptr;


int32_t (*g_pfnGetUnkTimer)(CVehicle*);
uint8_t (*g_pfnGetVehicleFlags2_1)(CVehicle*);
uint8_t (*g_pfnGetVehicleFlags1_1)(CVehicle*);
uint8_t(*g_pfnGetForceCarLightMask)(CVehicle*);

float*(*g_pfnGetAutomobileUnkCannonVal)(CVehicle*);

CPed* (*g_pfnGetDriver)(CVehicle*);
CVehicle* (*g_pfnGetVehicle)(CPed*);


namespace legacy_fns {

float getRpm(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0x10E0 + 4); }
uint32_t getNumWheels(CVehicle* pVeh) { return *(uint32_t*)((uint8_t*)pVeh + 0xFD4); }
CWheel* getWheels(CVehicle* pVeh) { return *(CWheel**)((uint8_t*)pVeh + 0xFD0); }
float getTemp(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0xF84); }
CHandlingVehicle* getHandlingData(CVehicle* pVeh) { return *(CHandlingVehicle**)((uint8_t*)pVeh + 0xE18); }
char getVehFlags1_0(CVehicle* pVeh) { return *((uint8_t*)pVeh + 0xF64); }
float getThrottle(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0x10E0 + 0x14); }
uint32_t getSpecialType(CVehicle* pVeh) { return *(uint32_t*)((uint8_t*)pVeh + 0x1354); }
uint32_t getMainType(CVehicle* pVeh) { return *(uint32_t*)((uint8_t*)pVeh + 0x1350); }
float getGasPedal(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0x10C8); }
float getBrakePedal(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0x10CC); }
uint32_t get_FE0(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0xFE0); }
fragInst* getVehFragInst(CVehicle* pVeh) { return *(fragInst**)((uint8_t*)pVeh + 0xE14); }

int32_t getGetUnkTimer(CVehicle* pVeh) { return *(int32_t*)((uint8_t*)pVeh + 0xF7C); }
uint8_t getVehicleFlags2_1(CVehicle* pVeh) { return *(uint8_t*)((uint8_t*)pVeh + 0xF69); }
uint8_t getVehicleFlags1_1(CVehicle* pVeh) { return *(uint8_t*)((uint8_t*)pVeh + 0xF65); }
uint8_t getForceCarLightMask(CVehicle* pVeh) { return *(uint8_t*)((uint8_t*)pVeh + 0x1112); }
float* getAutomobileUnkCannonVal(CVehicle* pVeh) { return (float*)((uint8_t*)pVeh + 0x14E8); }
CPed* getDriver(CVehicle* pVeh) { return *(CPed**)((uint8_t*)pVeh + 0xFA0); }
CVehicle* getVehicle(CPed* pPed) { return *(CVehicle**)((uint8_t*)pPed + 0xB40); }

void set() {
	g_pfnGetVehRpm = getRpm;
	g_pfnGetVehNumWheels = getNumWheels;
	g_pfnGetVehWheels = getWheels;
	g_pfnGetVehTemp = getTemp;
	g_pfnGetHandlingData = getHandlingData;
	g_pfnGetVehFlags1_0 = getVehFlags1_0;
	g_pfnGetVehThrottle = getThrottle;
	g_pfnGetVehSpecialType = getSpecialType;
	g_pfnGetVehGasPedal = getGasPedal;
	g_pfnGetVehBrakePedal = getBrakePedal;
	g_pfnGetVeh_FE0 = get_FE0;
	g_pfnGetVehFragInst = getVehFragInst;
	g_pfnGetVehMainType = getMainType;

	g_pfnGetUnkTimer = getGetUnkTimer;
	g_pfnGetVehicleFlags2_1 = getVehicleFlags2_1;
	g_pfnGetVehicleFlags1_1 = getVehicleFlags1_1;
	g_pfnGetForceCarLightMask = getForceCarLightMask;
	g_pfnGetAutomobileUnkCannonVal = getAutomobileUnkCannonVal;
	g_pfnGetDriver = getDriver;
	g_pfnGetVehicle = getVehicle;

}

}
namespace ce_fns {

float getRpm(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0x10E0 + 4 - 0x50); }
uint32_t getNumWheels(CVehicle* pVeh) { return *(uint32_t*)((uint8_t*)pVeh + 0xFD4 - 0x50); }
CWheel* getWheels(CVehicle* pVeh) { return *(CWheel**)((uint8_t*)pVeh + 0xFD0 - 0x50); }
float getTemp(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0xF84 - 0x50); }
CHandlingVehicle* getHandlingData(CVehicle* pVeh) { return *(CHandlingVehicle**)((uint8_t*)pVeh + 0xE18 - 0x50); }
char getVehFlags1_0(CVehicle* pVeh) { return *((uint8_t*)pVeh + 0xF64 - 0x50); }
float getThrottle(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0x10E0 + 0x14 - 0x50); }
uint32_t getSpecialType(CVehicle* pVeh) { return *(uint32_t*)((uint8_t*)pVeh + 0x1354 - 0x50); }
uint32_t getMainType(CVehicle* pVeh) { return *(uint32_t*)((uint8_t*)pVeh + 0x1350 - 0x50); }
float getGasPedal(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0x10C8 - 0x50); }
float getBrakePedal(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0x10CC - 0x50); }
uint32_t get_FE0(CVehicle* pVeh) { return *(float*)((uint8_t*)pVeh + 0xFE0 - 0x50); }
fragInst* getVehFragInst(CVehicle* pVeh) { return *(fragInst**)((uint8_t*)pVeh + 0xE14 - 0x50); }

int32_t getGetUnkTimer(CVehicle* pVeh) { return *(int32_t*)((uint8_t*)pVeh + 0xF7C - 0x50); }
uint8_t getVehicleFlags2_1(CVehicle* pVeh) { return *(uint8_t*)((uint8_t*)pVeh + 0xF69 - 0x50); }
uint8_t getVehicleFlags1_1(CVehicle* pVeh) { return *(uint8_t*)((uint8_t*)pVeh + 0xF65 - 0x50); }
uint8_t getForceCarLightMask(CVehicle* pVeh) { return *(uint8_t*)((uint8_t*)pVeh + 0x1112 - 0x50); }
float* getAutomobileUnkCannonVal(CVehicle* pVeh) { return (float*)((uint8_t*)pVeh + 0x14E8 - 0x50); }

CPed* getDriver(CVehicle* pVeh) { return *(CPed**)((uint8_t*)pVeh + 0xFA0 - 0x50); }
CVehicle* getVehicle(CPed* pPed) { return *(CVehicle**)((uint8_t*)pPed + 0xB30); }

void set() {
	g_pfnGetVehRpm = getRpm;
	g_pfnGetVehNumWheels = getNumWheels;
	g_pfnGetVehWheels = getWheels;
	g_pfnGetVehTemp = getTemp;
	g_pfnGetHandlingData = getHandlingData;
	g_pfnGetVehFlags1_0 = getVehFlags1_0;
	g_pfnGetVehThrottle = getThrottle;
	g_pfnGetVehSpecialType = getSpecialType;
	g_pfnGetVehGasPedal = getGasPedal;
	g_pfnGetVehBrakePedal = getBrakePedal;
	g_pfnGetVeh_FE0 = get_FE0;
	g_pfnGetVehFragInst = getVehFragInst;
	g_pfnGetVehMainType = getMainType;

	g_pfnGetUnkTimer = getGetUnkTimer;
	g_pfnGetVehicleFlags2_1 = getVehicleFlags2_1;
	g_pfnGetVehicleFlags1_1 = getVehicleFlags1_1;
	g_pfnGetForceCarLightMask = getForceCarLightMask;

	g_pfnGetAutomobileUnkCannonVal = getAutomobileUnkCannonVal;
	g_pfnGetDriver = getDriver;
	g_pfnGetVehicle = getVehicle;
}

}


void initHelpers() {
	if (g_bIsCE)
		ce_fns::set();
	else
		legacy_fns::set();

}