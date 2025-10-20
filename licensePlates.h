#pragma once
#include "vehExtParams.h"

struct CCustomShaderEffectVehicleFX;
struct CVehicle;

void __cdecl updateLicensePlates(CCustomShaderEffectVehicleFX*, CVehicle*, CVehExtParams::Params*, CCustomShaderEffectVehicleFX::CAddonVars*);
void __cdecl setVarsLicensePlates(CCustomShaderEffectVehicleFX*, CCustomShaderEffectVehicleFX::CAddonVars*);
void __cdecl initLicensePlates(CCustomShaderEffectVehicleFX*, CCustomShaderEffectVehicleFX::CAddonVars*);
void __cdecl setStandartLicensePlate(CVehicle* pVeh, CVehExtParams::Params* pParams);

void parseCarPlatesIde(char* line);
