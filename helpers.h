#pragma once
#include <inttypes.h>

struct CVehicle;
struct CWheel;
struct CHandlingVehicle;
struct fragInst;
struct CPed;

extern float (*g_pfnGetVehRpm)(CVehicle*);
extern uint32_t(*g_pfnGetVehNumWheels)(CVehicle*);
extern CWheel* (*g_pfnGetVehWheels)(CVehicle*);
extern float(*g_pfnGetVehTemp)(CVehicle*);
extern CHandlingVehicle* (*g_pfnGetHandlingData)(CVehicle*);
extern char(*g_pfnGetVehFlags1_0)(CVehicle*);
extern float(*g_pfnGetVehThrottle)(CVehicle*);
extern uint32_t(*g_pfnGetVehSpecialType)(CVehicle*);
extern uint32_t(*g_pfnGetVehMainType)(CVehicle*);
extern float(*g_pfnGetVehGasPedal)(CVehicle*);
extern float(*g_pfnGetVehBrakePedal)(CVehicle*);
extern uint32_t(*g_pfnGetVeh_FE0)(CVehicle*);
extern fragInst* (*g_pfnGetVehFragInst)(CVehicle*);

extern int32_t (*g_pfnGetUnkTimer)(CVehicle*);
extern uint8_t (*g_pfnGetVehicleFlags2_1)(CVehicle*);
extern uint8_t (*g_pfnGetVehicleFlags1_1)(CVehicle*);
extern uint8_t (*g_pfnGetForceCarLightMask)(CVehicle*);
extern float *(*g_pfnGetAutomobileUnkCannonVal)(CVehicle*);
extern CPed *(*g_pfnGetDriver)(CVehicle*);
extern CVehicle*(*g_pfnGetVehicle)(CPed*);

namespace missing_funcs {
struct CPed_CE_getIsPlayer {
	uint8_t __0[0x300];

	bool getIsPlayer() { return !__0[0x218] && __0[0x219]; }

};

}

void initHelpers();
