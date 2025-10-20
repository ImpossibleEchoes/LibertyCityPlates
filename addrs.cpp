#include "addrs.h"
#include "hookFns.h"
#include "helpers.h"
#include "Hooking.Patterns-master/Hooking.Patterns.h"

struct Matrix34;

size_t g_hookAddr_CVehicle__CVehicle; // call opcode
//size_t g_atPool__indexOf;
size_t* g_pCVehicle__ms_pPool; // указатель на указатель. 
size_t g_rage__grmShaderGroup__addShaderGroupVar;
size_t g_vmtAddr__CCustomShaderEffectVehicleFX__init;
size_t g_vmtAddr__CCustomShaderEffectVehicleFX__update;
size_t g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars;

size_t g_hookAddr__CCustomShaderEffectVehicleFX__init;

size_t g_hookAddr_createCustomVehFx;

size_t g_CCustomShaderEffectVehicleFX_struct_size;

size_t g_rage__grmShaderGroup__SetVarFloat;
size_t g_rage__grmShaderGroup__SetVarVector4;

size_t g_vmtAddr__CVehicleFactoryNY__createVehicle;

size_t g_rand;
size_t g_hookAddr_registerNative;

// добавить эти(ниже)
size_t g_hookAddr_CVehicle__prerender;
size_t g_CDynamicEntity__getSkeletonData;
size_t g_CDynamicEntity__getLocalMatrix;
size_t g_hookAddr_CVehicle__processControl;
float* g_pfTimeStep;
uint32_t* g_pdwGameTimer;
size_t g_isGameKeyboardKeyJustPressed;
size_t g_hookAddr_CTransmission__process;
size_t g_CAutomobile__processPhysics_engineOnCheckJump;
size_t g_hookAddr_CBaseModelInfo__addCustomShaderEffect;
size_t g_addCustomShaderEffect_vehShadersCheck;

size_t g_fragInstGta__getFragType;
CVehicleModelInfo** g_modelPointers;
size_t g_fragType__getChildIndexByBoneIndex;
size_t g_vmtAddr_CVehicle__setVehComponentsColFalgs;
size_t g_fragType__getGroupIndexByBoneIndex;
size_t g_hookAddr_CVehicleModelInfo__setPhysics;

size_t g_getDistSqrToCamera;
size_t g_ppMaterialMgr;
size_t g_pVehFx;
size_t g_pUnkVisualEffectObject; // byte_1174B20
size_t g_unkVisualEffectFn1; // sub_7FAA80
size_t g_unkVisualEffectFn2; // sub_8BBC20
size_t g_hookAddr_CWheel__processSkidmarkVFx;

size_t g_CVehicle__setBoneRotation;

size_t g_CPed__getPad2;

size_t g_hookAddr_readVehFlags;

size_t g_hookAddr_initAutomobileRWheels[2];
size_t g_vmtAddr_automobileProcessPhysics;

size_t g_CWeapon__constructor;
size_t g_CWeapon__destructor;
size_t g_CWeapon__init;
size_t g_CWeapon__fire;
size_t g_CWeapon__update;
size_t g_gta_fopen;
size_t g_gta_fclose;
size_t g_hookAddr_readIde;
size_t g_getWeaponInfoIdByName;
size_t g_hookAddr_CVehicle__destructor;
uint32_t **g_unkObject;
size_t g_unkCall;
Matrix34* g_unkMatrix34;
size_t g_Matrix34__getEuler;
size_t g_Matrix34__rotateLocalX;
size_t g_CPed__getIsPlayer;
size_t g_vmtAddr_CAutomobile__processCannon;
size_t g_vmtAddr_CHeli__processCannon;
size_t g_vmtAddr_CPlane__processCannon;
size_t g_hookAddr_pedShotBlocking;

// конец
CHandlingVehicle* g_handling;

bool g_bIsCE = false;

size_t findPattern(const char* pszPattern, ptrdiff_t offset = 0) {
	size_t found_address = 0;
//#ifdef _DEBUG
	hook::pattern g = hook::pattern(pszPattern); // .count(1);
//#else
//	hook::pattern g = hook::pattern(pszPattern).count(1);
//#endif
	if (!g.empty())
		found_address = reinterpret_cast<size_t>(g.get(0).get<void>(offset));



	return found_address;
}

// ToDo: add CVehicle::setBoneRotation addr
uint32_t initAddrsDynamicLegacy() {

	// check for CE
	hook::pattern g = hook::pattern("E8 ? ? ? ? 8D 8E ? ? ? ? C7 06 ? ? ? ? E8 ? ? ? ? 8D 8E ? ? ? ? E8 ? ? ? ? 8D BE ? ? ? ? 8B CF E8 ? ? ? ?");
	if (g.empty())
		return 1;

	uint32_t result = 0;

	g_hookAddr_CVehicle__CVehicle = findPattern("E8 ? ? ? ? 8D 8E ? ? ? ? C7 06 ? ? ? ? E8 ? ? ? ? 8D 8E ? ? ? ? E8 ? ? ? ? 8D BE ? ? ? ? 8B CF E8 ? ? ? ?");
	if (!g_hookAddr_CVehicle__CVehicle)
		result |= 1;

	if (result)
		return 1;


	g_pCVehicle__ms_pPool = (size_t*)findPattern("8B 0D ? ? ? ? 68 ? ? ? ? F3 0F 11 05 ? ? ? ? A2 ? ? ? ? C6 05 ? ? ? ? ? 88 15 ? ? ? ? E8 ? ? ? ? C3", 2);
	if (g_pCVehicle__ms_pPool)
		g_pCVehicle__ms_pPool = *(size_t**)g_pCVehicle__ms_pPool;
	else
		result |= 1 << 2;

	g_rage__grmShaderGroup__addShaderGroupVar = findPattern("55 8B EC 83 EC 18 53 8B D9 66 8B 43 14 66 3B 43 3C 56 57 89 5D FC 0F 84 ? ? ? ? 0F B7 4B 16 8B 7B 10 51 E8 ? ? ? ? 33 FF 89 7B 10 ");
	if (!g_rage__grmShaderGroup__addShaderGroupVar)
		result |= 1 << 3;

	g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars = 
		findPattern("8B 44 24 04 56 50 8B F1 E8 ? ? ? ? 33 C0 89 86 ? ? ? ? 89 86 ? ? ? ? 89 86 ? ? ? ? 89 86 ? ? ? ? C7 06 ? ? ? ? C7 86 ? ? ? ? ? ? ? ? 8B C6 5E C2 04 00", 0x27 + 2);
	if (g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars) {
		size_t* vmt = *(size_t**)g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars;

		g_vmtAddr__CCustomShaderEffectVehicleFX__init = (size_t)(vmt + 1);
		g_vmtAddr__CCustomShaderEffectVehicleFX__update = (size_t)(vmt + 3);
		g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars = (size_t)(vmt + 4);
	}
	else
		result |= 1 << 4;

	g_CCustomShaderEffectVehicleFX_struct_size = findPattern("81 C6 ? ? ? ? 83 E6 F0 56 89 44 24 20 89 4C 24 28 E8 ? ? ? ? 83 C4 04 85 C0 74 0C 56 8B C8 ", 2);
	if (!g_CCustomShaderEffectVehicleFX_struct_size)
		result |= 1 << 5;

	g_rage__grmShaderGroup__SetVarFloat = findPattern("53 56 57 8B F9 0F B7 5F 0C 33 F6 85 DB 7E 4B F3 0F 10 44 24 ? 55");
	if (!g_rage__grmShaderGroup__SetVarFloat)
		result |= 1 << 6;

	g_rage__grmShaderGroup__SetVarVector4 = findPattern("55 56 57 8B F9 0F B7 6F 0C 33 F6 85 ED 7E 39 53 8B 5C 24 18 8B 4C 24 14 8B 47 38 8B 54 C8 F8 8B 0C B2 85 C9 74 1A 8B 47 08 8B 04 B0 6A 05 6A 01 6A 10 ");
	if (!g_rage__grmShaderGroup__SetVarVector4)
		result |= 1 << 7;

	g_vmtAddr__CVehicleFactoryNY__createVehicle = 
		findPattern("C7 06 ? ? ? ? 89 35 ? ? ? ? EB 06 89 1D ? ? ? ? 64 A1 ? ? ? ? 8B 08 8B 49 08 8B 11 8B 42 08 53 6A 10 6A 04 FF D0 3B C3 74 10 57 8B F8 E8 ? ? ? ? A3 ? ? ? ? 5F ", 2);
	if (g_vmtAddr__CVehicleFactoryNY__createVehicle) {
		size_t* vmt = *(size_t**)g_vmtAddr__CVehicleFactoryNY__createVehicle;

		g_vmtAddr__CVehicleFactoryNY__createVehicle = (size_t)(vmt + 1);
	}
	else
		result |= 1 << 8;

	g_rand = findPattern("E8 ? ? ? ? 8B 48 14 69 C9 ? ? ? ? 81 C1 ? ? ? ? 89 48 14 8B C1 C1 E8 10 25 ? ? ? ? C3");
	if (!g_rand)
		result |= 1 << 9;

	g_hookAddr_registerNative = findPattern("68 F7 37 85 7B E8 ? ? ? ?", 5);
	if (!g_hookAddr_registerNative)
		result |= 1 << 10;


	//
	g_hookAddr_CVehicle__prerender = findPattern("E8 ? ? ? ? 0F B6 C8 C1 E1 1C 33 4E 28 81 E1 ? ? ? ? 31 4E 28 84 C0 8B 4E 28 ");
	if (!g_hookAddr_CVehicle__prerender)
		result |= 1 << 11;

	g_CDynamicEntity__getSkeletonData = findPattern("56 8B F1 8B 06 8B 90 ? ? ? ? FF D2 85 C0 74 1D 8B 06 ");
	if (!g_CDynamicEntity__getSkeletonData)
		result |= 1 << 12;

	g_CDynamicEntity__getLocalMatrix = findPattern("56 8B F1 8B 06 8B 90 ? ? ? ? 57 FF D2 85 C0 74 1C 8B 06 8B 90 ? ? ? ? 8B CE FF D2 8B 10 8B C8 8B 82 ? ? ? ? FF D0 8B F8 EB 06 ");
	if (!g_CDynamicEntity__getLocalMatrix)
		result |= 1 << 13;

	g_hookAddr_CVehicle__processControl = findPattern("3B BA ? ? ? ? 7C C7 8D 4E 10 E8 ? ? ? ? 5F 5E 5B 8B E5 5D C3 ", 0xB);
	if (!g_hookAddr_CVehicle__processControl)
		result |= 1 << 14;

	g_pfTimeStep = (float*)findPattern("F3 0F 10 05 ? ? ? ? 8D 4C 24 10 51 8B CE F3 0F 11 44 24 ? 76 2B FF D2 F3 0F 10 08 F3 0F 10 40 ? 0F 28 D1 F3 0F 59 D1 ", 4);
	if (g_pfTimeStep)
		g_pfTimeStep = *reinterpret_cast<float**>(g_pfTimeStep);
	else
		result |= 1 << 15;



	g_pdwGameTimer = (uint32_t*)findPattern("8B 3D ? ? ? ? 74 16 A8 04 74 12 8B 51 18 81 C2 ? ? ? ? 3B FA C6 44 24 ? ? 72 05 ", 2);
	if (g_pdwGameTimer)
		g_pdwGameTimer = *(uint32_t**)g_pdwGameTimer;
	else
		result |= 1 << 16;

	g_hookAddr_CTransmission__process = findPattern("E8 ? ? ? ? D9 5C 24 20 33 FF 39 BE ? ? ? ? 7E 35 33 DB 3B BE ? ? ? ? 7D 0A ");
	if (!g_hookAddr_CTransmission__process)
		result |= 1 << 17;

	g_CAutomobile__processPhysics_engineOnCheckJump = findPattern("0F 84 ? ? ? ? 8B 86 ? ? ? ? F7 80 ? ? ? ? ? ? ? ? 0F 85 ? ? ? ? 8B BE ? ? ? ? 33 D2 33 DB 33 C9 3B FA 89 54 24 1C 0F 8E ? ? ? ? ", 2);
	if (!g_CAutomobile__processPhysics_engineOnCheckJump)
		result |= 1 << 18;

	g_hookAddr_CBaseModelInfo__addCustomShaderEffect = findPattern("E8 ? ? ? ? 8B 46 40 A9 ? ? ? ? 74 08 25 ? ? ? ? 89 46 40 8B 4E 08 ");
	if (!g_hookAddr_CBaseModelInfo__addCustomShaderEffect)
		result |= 1 << 19;

	g_addCustomShaderEffect_vehShadersCheck = findPattern("68 ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 7F 03 00 00 8B 4E 08 68 ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? ");
	if (!g_addCustomShaderEffect_vehShadersCheck)
		result |= 1 << 20;



	g_fragInstGta__getFragType = findPattern("8B 41 5C 85 C0 74 17 8B 48 08 81 E1 ? ? ? ? 81 F9 ? ? ? ? 74 03 33 C0 C3 ");
	if (!g_fragInstGta__getFragType)
		result |= 1 << 21;

	g_modelPointers = (CVehicleModelInfo**)findPattern("8B 04 95 ? ? ? ? 8B 0F 8B 90 ? ? ? ? 55 8B 2C 8A 83 FD FF 0F 8E ? ? ? ? F3 0F 10 47 ? 0F 57 C9 F3 0F 10 1D ? ? ? ? ", 3);
	if (g_modelPointers)
		g_modelPointers = *(CVehicleModelInfo***)g_modelPointers;
	else
		result |= 1 << 22;

	g_fragType__getChildIndexByBoneIndex = findPattern("8B 4C 24 04 85 C9 56 57 74 2B 0F B6 91 ? ? ? ? 33 C0 85 D2 7E 1E 8B 89 ? ? ? ? 8B 74 24 10 8B 39 0F BF 7F 0E 3B FE 74 0D ");
	if (!g_fragType__getChildIndexByBoneIndex)
		result |= 1 << 23;

	g_vmtAddr_CVehicle__setVehComponentsColFalgs = 
		findPattern("C7 06 ? ? ? ? 8D BE ? ? ? ? BB ? ? ? ? 8B CF E8 ? ? ? ? 81 C7 ? ? ? ? 83 EB 01 79 EE 8D BE ? ? ? ? BB ? ? ? ? 8B CF E8 ? ? ? ? 83 C7 34 83 EB 01 79 F1 0F 57 C0 ", 2);
	if (g_vmtAddr_CVehicle__setVehComponentsColFalgs) {
		size_t* vmt = *(size_t**)g_vmtAddr_CVehicle__setVehComponentsColFalgs;

		g_vmtAddr_CVehicle__setVehComponentsColFalgs = (size_t)(vmt + 0x120 / sizeof size_t);
	}
	else
		result |= 1 << 24;

	g_fragType__getGroupIndexByBoneIndex = findPattern("8B 4C 24 04 85 C9 53 56 57 74 2D 0F B6 91 ? ? ? ? 33 C0 85 D2 7E 20 8B B1 ? ? ? ? 8B 7C 24 14 8B CE ");
	if (!g_fragType__getGroupIndexByBoneIndex)
		result |= 1 << 25;

	g_hookAddr_CVehicleModelInfo__setPhysics = findPattern("E8 ? ? ? ? 8D 4C 24 60 51 57 8B CE E8 ? ? ? ? DD D8 8B CE E8 ? ? ? ? ");
	if (!g_hookAddr_CVehicleModelInfo__setPhysics)
		result |= 1 << 26;

	g_handling = (CHandlingVehicle*)findPattern("8D B0 ? ? ? ? 53 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? ", 2);
	if (g_handling)
		g_handling = *(CHandlingVehicle**)g_handling;
	else
		result |= 1 << 27;


	g_ppMaterialMgr = findPattern("8B 0D ? ? ? ? 50 E8 ? ? ? ? 83 F8 17 74 26 D9 86 ? ? ? ? ", 2);
	if (g_ppMaterialMgr)
		g_ppMaterialMgr = *(size_t*)g_ppMaterialMgr;
	else
		result |= 1 << 28;

	g_getDistSqrToCamera = findPattern("E8 ? ? ? ? D9 45 14 D8 0D ? ? ? ? 83 C4 04 D9 C9 DF F1 DD D8 0F 87 ? ? ? ? ");
	if (g_getDistSqrToCamera)
		g_getDistSqrToCamera = getFnAddrInCallOpcode(g_getDistSqrToCamera);
	else
		result |= 1 << 29;

	g_pVehFx = findPattern("B9 ? ? ? ? E8 ? ? ? ? 8B BE ? ? ? ? 8B D8 F3 0F 10 1B 8B C7 C1 E8 04 ", 1);
	if (g_pVehFx)
		g_pVehFx = *(size_t*)g_pVehFx;
	else
		result |= 1 << 30;

	g_pUnkVisualEffectObject = findPattern("B9 ? ? ? ? E8 ? ? ? ? 83 BF ? ? ? ? ? 0F 84 ? ? ? ? 8B 86 ? ? ? ? 8B C8 C1 E9 0C F6 C1 01 0F 84 ? ? ? ? ", 1);
	if (g_pUnkVisualEffectObject)
		g_pUnkVisualEffectObject = *(size_t*)g_pUnkVisualEffectObject;
	else
		result |= 1 << 31;

	g_unkVisualEffectFn1 = findPattern("E8 ? ? ? ? 83 C4 2C 6A 00 8D 54 24 44 52 B9 ? ? ? ? E8 ? ? ? ? 83 BF ? ? ? ? ? 0F 84 ? ? ? ? ");
	if (g_unkVisualEffectFn1)
		g_unkVisualEffectFn1 = getFnAddrInCallOpcode(g_unkVisualEffectFn1);
	else
		result |= 1 << 29;

	g_unkVisualEffectFn2 = findPattern("E8 ? ? ? ? 83 BF ? ? ? ? ? 0F 84 ? ? ? ? 8B 86 ? ? ? ? 8B C8 C1 E9 0C F6 C1 01 0F 84 ? ? ? ? ");
	if (g_unkVisualEffectFn2)
		g_unkVisualEffectFn2 = getFnAddrInCallOpcode(g_unkVisualEffectFn2);
	else
		result |= 1 << 29;

	g_hookAddr_CWheel__processSkidmarkVFx = findPattern("E8 ? ? ? ? 5B 8B 96 ? ? ? ? C1 EA 03 F6 C2 01 75 11 F3 0F 10 86 ? ? ? ? ");
	if (!g_hookAddr_CWheel__processSkidmarkVFx)
		result |= 1 << 30;

	g_CVehicle__setBoneRotation = findPattern("55 8B EC 83 E4 F0 51 53 8B D9 8B 03 8B 90 ? ? ? ? 56 57 ");
	if (!g_CVehicle__setBoneRotation) 
		result |= 1 << 8;

	g_CPed__getPad2 = findPattern("E8 ? ? ? ? F6 46 68 01 8B D8 57 8B CE 74 07 E8 ? ? ? ? EB 05 ");
	if (g_CPed__getPad2)
		g_CPed__getPad2 = getFnAddrInCallOpcode(g_CPed__getPad2);
	else
		result |= 1 << 30;

	g_hookAddr_readVehFlags = findPattern("8B CE E8 ? ? ? ? 8B 8C 24 ? ? ? ? 5F 5E 5D 33 CC E8 ? ? ? ? 81 C4 ? ? ? ? ", 2);
	if (!g_hookAddr_readVehFlags)
		result |= 1 << 30;

	g_hookAddr_initAutomobileRWheels[0] = findPattern("E8 ? ? ? ? 83 86 ? ? ? ? ? BF ? ? ? ? 0F BF 56 2E ");
	g_hookAddr_initAutomobileRWheels[1] = findPattern("E8 ? ? ? ? 01 BE ? ? ? ? 8B 95 ? ? ? ? ");
	if (!g_hookAddr_initAutomobileRWheels[0] || !g_hookAddr_initAutomobileRWheels[1])
		result |= 1 << 30;

	g_vmtAddr_automobileProcessPhysics = findPattern("C7 06 ? ? ? ? 8D BE ? ? ? ? BB ? ? ? ? 8B CF E8 ? ? ? ? 81 C7 ? ? ? ? 83 EB 01 79 EE 8D BE ? ? ? ? ", 2);
	if (g_vmtAddr_automobileProcessPhysics) {
		size_t* vmt = *(size_t**)g_vmtAddr_automobileProcessPhysics;

		g_vmtAddr_automobileProcessPhysics = (size_t)(vmt + 0x104 / sizeof size_t);
	}
	else
		result |= 1 << 30;

	g_CWeapon__constructor = findPattern("53 56 8B F1 E8 ? ? ? ? 33 DB 53 8D 4E 5C ");
	if (!g_CWeapon__constructor)
		result |= 1 << 30;

	g_CWeapon__destructor = findPattern("E8 ? ? ? ? 57 E8 ? ? ? ? 83 C4 04 89 9E ? ? ? ? 8D BE ? ? ? ? ");
	if (g_CWeapon__destructor)
		g_CWeapon__destructor = getFnAddrInCallOpcode(g_CWeapon__destructor);
	else
		result |= 1 << 30;

	g_CWeapon__init = findPattern("8B 44 24 04 56 8B F1 89 46 18 8B 44 24 0C 66 3D FF 7F 76 07 B8 ? ? ? ? EB 03 ");
	if (!g_CWeapon__init)
		result |= 1 << 30;

	g_CWeapon__fire = findPattern("55 8B EC 83 E4 F0 83 EC 34 53 56 8B D9 8B 43 18 57 50 E8 ? ? ? ? ");
	if (!g_CWeapon__fire)
		result |= 1 << 30;

	g_CWeapon__update = findPattern("56 8B F1 8B 46 1C 83 F8 05 0F 87 ? ? ? ? FF 24 85 ? ? ? ? ");
	if (!g_CWeapon__update)
		result |= 1 << 30;

	g_gta_fopen = findPattern("81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 8B 8C 24 ? ? ? ? 32 D2 85 C9 ");
	if (!g_gta_fopen)
		result |= 1 << 30;

	g_gta_fclose = findPattern("E8 ? ? ? ? 55 E8 ? ? ? ? 83 C4 08 5E 5D 8A C3 5B 5F 59 ");
	if (g_gta_fclose)
		g_gta_fclose = getFnAddrInCallOpcode(g_gta_fclose);
	else
		result |= 1 << 30;

	g_hookAddr_readIde = findPattern("E8 ? ? ? ? 8B D8 83 C4 08 85 DB 89 5C 24 14 0F 84 ? ? ? ? 8B C6 8D 49 00 ");
	if (!g_hookAddr_readIde)
		result |= 1 << 30;

	g_getWeaponInfoIdByName = findPattern("E8 ? ? ? ? 83 C4 08 3B F0 0F 85 ? ? ? ? 5F");
	if (g_getWeaponInfoIdByName)
		g_getWeaponInfoIdByName = getFnAddrInCallOpcode(g_getWeaponInfoIdByName);
	else
		result |= 1 << 30;

	g_hookAddr_CVehicle__destructor = findPattern("E8 ? ? ? ? 83 C4 08 8B CE 66 C7 86 ? ? ? ? ? ? ");
	if (!g_hookAddr_CVehicle__destructor)
		result |= 1 << 30;

	g_unkObject = (uint32_t**)findPattern("B9 ? ? ? ? E8 ? ? ? ? 8B C8 E8 ? ? ? ? 85 C0 74 36 0F BE 88 ? ? ? ? 83 E9 01 74 14 83 E9 01 ", 1);
	if (g_unkObject)
		g_unkObject = *(uint32_t***)g_unkObject;
	else
		result |= 1 << 30;

	g_unkCall = findPattern("E8 ? ? ? ? 85 C0 74 36 0F BE 88 ? ? ? ? 83 E9 01 74 14 83 E9 01 ");
	if (g_unkCall)
		g_unkCall = getFnAddrInCallOpcode(g_unkCall);
	else
		result |= 1 << 30;

	g_unkMatrix34 = (Matrix34*)findPattern("68 ? ? ? ? 8D 4C 24 34 E8 ? ? ? ? 8D 4C 24 70 51 ", 1);
	if (g_unkMatrix34)
		g_unkMatrix34 = *(Matrix34**)g_unkMatrix34;
	else
		result |= 1 << 30;

	g_Matrix34__getEuler = findPattern("E8 ? ? ? ? D9 44 24 70 51 D9 1C 24 E8 ? ? ? ? ");
	if (g_Matrix34__getEuler)
		g_Matrix34__getEuler = getFnAddrInCallOpcode(g_Matrix34__getEuler);
	else
		result |= 1 << 30;

	g_Matrix34__rotateLocalX = findPattern("E8 ? ? ? ? F3 0F 10 5C 24 ? F3 0F 10 64 24 ? F3 0F 10 6C 24 ? F3 0F 10 54 24 ? 0F 28 C3 ");
	if (g_Matrix34__rotateLocalX)
		g_Matrix34__rotateLocalX = getFnAddrInCallOpcode(g_Matrix34__rotateLocalX);
	else
		result |= 1 << 30;

	g_CPed__getIsPlayer = findPattern("80 B9 ? ? ? ? ? 75 0F 80 B9 ? ? ? ? ? 74 06 B8 ? ? ? ? C3 ");
	if (!g_CPed__getIsPlayer)
		result |= 1 << 30;

	g_vmtAddr_CAutomobile__processCannon = findPattern("C7 06 ? ? ? ? 74 03 89 5E 38 39 9E ? ? ? ? BF ? ? ? ? ", 2);
	if (g_vmtAddr_CAutomobile__processCannon) {
		size_t* vmt = *(size_t**)g_vmtAddr_CAutomobile__processCannon;
		g_vmtAddr_CAutomobile__processCannon = (size_t)(vmt + 0x1C0 / sizeof size_t);
	}
	else
		result |= 1 << 8;

	g_vmtAddr_CHeli__processCannon = findPattern("C7 06 ? ? ? ? E8 ? ? ? ? 0F 57 C0 80 A6 ? ? ? ? ? F3 0F ", 2);
	if (g_vmtAddr_CHeli__processCannon) {
		size_t* vmt = *(size_t**)g_vmtAddr_CHeli__processCannon;
		g_vmtAddr_CHeli__processCannon = (size_t)(vmt + 0x1C0 / sizeof size_t);
	}
	else
		result |= 1 << 8;

	g_vmtAddr_CPlane__processCannon = findPattern("C7 06 ? ? ? ? C7 86 ? ? ? ? ? ? ? ? F3 0F 11 86 ? ? ? ? F3 0F 11 86 ? ? ", 2);
	if (g_vmtAddr_CPlane__processCannon) {
		size_t* vmt = *(size_t**)g_vmtAddr_CPlane__processCannon;
		g_vmtAddr_CPlane__processCannon = (size_t)(vmt + 0x1C0 / sizeof size_t);
	}
	else
		result |= 1 << 8;

	g_hookAddr_pedShotBlocking = findPattern("E8 ? ? ? ? 83 C4 04 84 C0 74 50 8B 86 ? ? ? ? 50 8D 4C 24 40 51 ");
	if (!g_hookAddr_pedShotBlocking)
		result |= 1 << 30;



	return result;
}


uint32_t initAddrsDynamicCE() {


	uint32_t result = 0;

	g_hookAddr_CVehicle__CVehicle = findPattern("E8 ? ? ? ? 8D 8F ? ? ? ? C7 07 ? ? ? ? E8 ? ? ? ? 8D 8F ? ? ? ? E8 ? ? ? ? 8D 8F ? ? ? ? E8 ? ? ? ? 8D 8F ? ? ? ? C7 87 ? ? ? ? ? ? ? ? ");
	if (!g_hookAddr_CVehicle__CVehicle)
		result |= 1;

	//addr = hook::pattern("8B 44 24 04 2B 01 99 F7 79 0C C2 04 00");
	//if (addr)
	//	g_atPool__indexOf = (size_t)p.get(0).get<void>();
	//else
	//	result |= 1 << 1;

	g_pCVehicle__ms_pPool = *(size_t**)findPattern("8B 0D ? ? ? ? F3 0F 11 05 ? ? ? ? F3 0F 10 00 F3 0F 11 05 ? ? ? ? F3 0F 11 0D ? ? ? ? F3 0F 11 15 ? ? ? ? F3 0F 10 40 ? 8A 44 24 0C A2 ? ? ? ? 8A 44 24 10 A2 ? ? ? ? 8A 44 24 14 ", 2);
	if (!g_pCVehicle__ms_pPool)
		result |= 1 << 2;

	g_rage__grmShaderGroup__addShaderGroupVar = findPattern("55 8B EC 83 EC 10 53 8B D9 56 66 8B 43 14 57 89 5D FC 66 3B 43 3C 0F 84 ? ? ? ? 0F B7 43 16 8D 73 10 50 FF 36");
	if (!g_rage__grmShaderGroup__addShaderGroupVar) {
		auto addr = findPattern("8B 44 24 04 56 8B F1 8B 48 08 6A 01 68 ? ? ? ? 89 4E 70 E8 ? ? ? ? 8B 4E 70 6A 00 68 ? ? ? ? 89 86 ? ? ? ? E8 ? ? ? ? 8B 4E 70 6A 00 68 ? ? ? ? ", 0x14);
		if (addr)
			g_rage__grmShaderGroup__addShaderGroupVar = getFnAddrInCallOpcode(addr);
		else
			result |= 1 << 3;
	}

	size_t addr = findPattern("56 FF 74 24 08 8B F1 E8 ? ? ? ? C7 06 ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? 8B C6 5E C2 04 00", 0xc + 2);
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmtAddr__CCustomShaderEffectVehicleFX__init = (size_t)(vmt + 1);
		g_vmtAddr__CCustomShaderEffectVehicleFX__update = (size_t)(vmt + 3);
		g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars = (size_t)(vmt + 4);

	}
	else
		result |= 1 << 4;

	addr = findPattern("81 C7 ? ? ? ? 83 E7 F0 57 E8 ? ? ? ? 83 C4 04 85 C0 74 0C 57 8B C8 E8 ? ? ? ? 8B F0 EB 02", 2);
	if (addr)
		g_CCustomShaderEffectVehicleFX_struct_size = addr;
	else
		result |= 1 << 5;

	addr = findPattern("56 8B F1 57 0F B7 7E 0C 33 D2 85 FF 7E 3F F3 0F 10 44 24 ? 53 8B 5C 24 10 8D A4 24 ? ? ? ? 8B 46 38 8B 44 D8 F8 8B 0C 90 85 C9 74 19");
	if (addr)
		g_rage__grmShaderGroup__SetVarFloat = addr;
	else {
		//addr = findPattern("E8 ? ? ? ? 0F B6 9E ? ? ? ? 33 FF 80 7E 78 00 74 44 85 DB 0F 8E ? ? ? ? EB 03 ");
		//	if (addr)
		//		g_rage__grmShaderGroup__SetVarFloat = getFnAddrInCallOpcode((size_t)p.get_first());
		//	else
				result |= 1 << 6;
	}

	addr = findPattern("53 8B D9 55 0F B7 6B 0C 57 33 FF 85 ED 7E 4C 8B 44 24 10 56 8D 0C C5 ? ? ? ? 89 4C 24 14 90 8B 43 38 8B 44 08 F8 8B 14 B8 85 D2 74 27 8B 43 08 BE 04 00 00 00");
	if (addr)
		g_rage__grmShaderGroup__SetVarVector4 = addr;
	else
		result |= 1 << 7;

	addr = findPattern("C7 06 ? ? ? ? 89 35 ? ? ? ? EB 0A C7 05 ? ? ? ? ? ? ? ? 8B 4F 08 6A 00 8B 01 6A 10 6A 04 FF 50 08 85 C0 74 0E 8B C8 E8 ? ? ? ? A3 ? ? ? ? EB 0A", 2);
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmtAddr__CVehicleFactoryNY__createVehicle = (size_t)(vmt + 1);

	}
	else
		result |= 1 << 8;

	addr = findPattern("E8 ? ? ? ? 8B 48 14 69 C9 ? ? ? ? 81 C1 ? ? ? ? 89 48 14 C1 E9 10 81 E1 ? ? ? ? 8B C1 C3");
	if (addr)
		g_rand = addr;
	else
		result |= 1 << 9;

	addr = findPattern("68 F7 37 85 7B E8 ? ? ? ?", 5);
	if (addr)
		g_hookAddr_registerNative = addr;
	else
		result |= 1 << 10;


	addr = findPattern("E8 ? ? ? ? 0F B6 C8 C1 E1 1C 33 4E 28 81 E1 ? ? ? ? 31 4E 28 8B 56 28 ");
	if (addr)
		g_hookAddr_CVehicle__prerender = addr;
	else
		result |= 1 << 11;

	addr = findPattern("56 8B F1 8B 06 FF 90 ? ? ? ? 85 C0 74 19 8B 06 8B CE FF 90 ? ? ? ? 8B 10 8B C8 FF 92 ? ? ? ? 8B 40 04 5E ");
	if (addr)
		g_CDynamicEntity__getSkeletonData = addr;
	else
		result |= 1 << 12;

	addr = findPattern("56 8B F1 57 8B 06 FF 90 ? ? ? ? 85 C0 74 18 8B 06 8B CE FF 90 ? ? ? ? 8B 10 8B C8 FF 92 ? ? ? ? 8B F8 ");
	if (addr)
		g_CDynamicEntity__getLocalMatrix = addr;
	else
		result |= 1 << 13;

	addr = findPattern("3B B8 ? ? ? ? 7C C5 8D 4E 10 E8 ? ? ? ? 5F 5E ", 0xB);
	if (addr)
		g_hookAddr_CVehicle__processControl = addr;
	else
		result |= 1 << 14;

	addr = findPattern("F3 0F 10 05 ? ? ? ? 51 8B CE F3 0F 11 44 24 ? 76 2E FF 90 ? ? ? ? F3 0F 10 00 F3 0F 10 48 ? F3 0F 59 C0 F3 0F 59 C9 F3 0F 58 C1 ", 4);
	if (addr)
		g_pfTimeStep = *reinterpret_cast<float**>(addr);
	else
		result |= 1 << 15;



	addr = findPattern("8B 3D ? ? ? ? F6 C2 02 74 16 F6 C2 04 74 11 8B 41 18 05 ? ? ? ? C6 44 24 ? ? 3B F8 72 05 ", 2);
	if (addr)
		g_pdwGameTimer = *(uint32_t**)addr;
	else
		result |= 1 << 16;

	addr = findPattern("E8 ? ? ? ? D9 5C 24 0C F3 0F 10 44 24 ? F3 0F 11 44 24 ? 33 F6 39 B7 ? ? ? ? 7E 42 33 C0 89 44 24 0C 3B B7 ? ? ? ? 7D 0A ");
	if (addr)
		g_hookAddr_CTransmission__process = addr;
	else
		result |= 1 << 17;

	addr = findPattern("0F 84 ? ? ? ? 8B 87 ? ? ? ? F7 80 ? ? ? ? ? ? ? ? 0F 85 ? ? ? ? 8B B7 ? ? ? ? 33 C0 33 D2 33 C9 89 44 24 08 89 54 24 10 85 F6 0F 8E ? ? ? ? 3B CE 7D 22 ", 2);
	if (addr)
		g_CAutomobile__processPhysics_engineOnCheckJump = addr;
	else
		result |= 1 << 18;

	addr = findPattern("E8 ? ? ? ? 8B 4E 0C 83 C4 14 89 41 04 80 7C 24 ? ? 74 1A FF 35 ? ? ? ? FF 76 3C E8 ? ? ? ? 83 C4 04 50 E8 ? ? ? ? 83 C4 08 ");
	if (addr)
		g_hookAddr_CBaseModelInfo__addCustomShaderEffect = addr;
	else
		result |= 1 << 19;

	addr = findPattern("68 ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 7F 03 00 00 8B 4F 08 68 ? ? ? ? E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? 8B 4F 08 68 ? ? ? ? E8 ? ? ? ? ");
	if (addr)
		g_addCustomShaderEffect_vehShadersCheck = addr;
	else
		result |= 1 << 20;



	addr = findPattern("8B 51 5C 85 D2 74 15 8B 42 08 25 ? ? ? ? 3D ? ? ? ? 74 03 33 C0 C3 "); //
	if (addr)
		g_fragInstGta__getFragType = addr;
	else
		result |= 1 << 21;

	addr = findPattern("8B 04 85 ? ? ? ? 8B 80 ? ? ? ? 8B 04 90 83 F8 FF 7E 41 50 8B 06 8B CE 8B 80 ? ? ? ? FF D0 8B C8 ", 3); //
	if (addr)
		//g_pModelPointers = *(size_t**)addr;
		g_modelPointers = *(CVehicleModelInfo***)addr;
	else
		result |= 1 << 22;

	addr = findPattern("8B 54 24 04 56 57 85 D2 74 29 0F B6 B2 ? ? ? ? 33 C9 85 F6 7E 1C 8B 92 ? ? ? ? 8B 7C 24 10 "); //
	if (addr)
		g_fragType__getChildIndexByBoneIndex = addr;
	else
		result |= 1 << 23;

	addr = findPattern("C7 06 ? ? ? ? 8D BE ? ? ? ? BE ? ? ? ? 8D 9B ? ? ? ? ", 2); //
	if (addr) {
		size_t* vmt = *(size_t**)addr;

		g_vmtAddr_CVehicle__setVehComponentsColFalgs = (size_t)(vmt + 0x120 / sizeof size_t);
	}
	else
		result |= 1 << 24;

	addr = findPattern("8B 44 24 04 53 56 57 85 C0 74 2B 0F B6 B0 ? ? ? ? 33 C9 85 F6 7E 1E ");
	if (addr)
		g_fragType__getGroupIndexByBoneIndex = addr;
	else
		result |= 1 << 25;

	addr = findPattern("E8 ? ? ? ? 8D 44 24 60 50 53 8B CF E8 ? ? ? ? DD D8 8B CF E8 ? ? ? ? ");
	if (addr)
		g_hookAddr_CVehicleModelInfo__setPhysics = addr;
	else
		result |= 1 << 26;

	addr = findPattern("8D B0 ? ? ? ? 57 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? ", 2);
	if (addr)
		g_handling = *(CHandlingVehicle**)addr;
	else
		result |= 1 << 27;



	addr = findPattern("8B 0D ? ? ? ? FF B6 ? ? ? ? 8B 01 8B 40 14 FF D0 66 83 78 ? ? 74 3E F3 0F 10 8E ? ? ? ? 0F 54 0D ? ? ? ? ", 2);
	if (addr)
		g_ppMaterialMgr = *(size_t*)addr;
	else
		result |= 1 << 28;

	addr = findPattern("E8 ? ? ? ? D9 5C 24 18 F3 0F 10 45 ? F3 0F 59 05 ? ? ? ? F3 0F 10 4C 24 ? 83 C4 04 0F 2F C8 0F 87 ? ? ? ? ");
	if (addr)
		g_getDistSqrToCamera = getFnAddrInCallOpcode(addr);
	else
		result |= 1 << 29;

	addr = findPattern("B9 ? ? ? ? C7 44 24 ? ? ? ? ? E8 ? ? ? ? 8B 7C 24 10 EB 42 ", 1);
	if (addr)
		g_pVehFx = *(size_t*)addr;
	else
		result |= 1 << 30;

	addr = findPattern("B9 ? ? ? ? E8 ? ? ? ? 8B 54 24 1C 83 BA ? ? ? ? ? 0F 84 ? ? ? ? 8B 8E ? ? ? ? 8B C1 C1 E8 0C ", 1);
	if (addr)
		g_pUnkVisualEffectObject = *(size_t*)addr;
	else
		result |= 1 << 31;

	addr = findPattern("E8 ? ? ? ? 83 C4 2C 8D 44 24 30 6A 00 50 B9 ? ? ? ? E8 ? ? ? ? 8B 54 24 1C 83 BA ? ? ? ? ? 0F 84 ? ? ? ? 8B 8E ? ? ? ? 8B C1 ");
	if (addr)
		g_unkVisualEffectFn1 = getFnAddrInCallOpcode(addr);
	else
		result |= 1 << 29;

	addr = findPattern("E8 ? ? ? ? 8B 54 24 1C 83 BA ? ? ? ? ? 0F 84 ? ? ? ? 8B 8E ? ? ? ? 8B C1 C1 E8 0C A8 01 0F 84 ? ? ? ? 0F BF 42 2E ");
	if (addr)
		g_unkVisualEffectFn2 = getFnAddrInCallOpcode(addr);
	else
		result |= 1 << 29;

	addr = findPattern("E8 ? ? ? ? 8B 87 ? ? ? ? C1 E8 03 A8 01 75 11 F3 0F 10 87 ? ? ? ? ");
	if (addr)
		g_hookAddr_CWheel__processSkidmarkVFx = addr;
	else
		result |= 1 << 30;


	addr = findPattern("51 56 8B F1 89 74 24 04 8B 06 8B 80 ? ? ? ? FF D0 85 C0 74 1A 8B 06 8B CE 8B 80 ? ? ? ? FF D0 8B 10 8B C8 8B 82 ? ? ? ? FF D0 EB 06 ");
	if (addr) {
		g_CVehicle__setBoneRotation = addr;
	}
	else
		result |= 1 << 8;

	addr = findPattern("E8 ? ? ? ? 89 44 24 20 85 C0 0F 84 ? ? ? ? 8B CF E8 ? ? ? ? 8B F0 32 DB 89 74 24 24 83 FE 03 7D 47 8B 17 8D 44 24 30 50 8B CF FF 92 ? ? ? ? ");
	if (addr) {
		g_CPed__getPad2 = getFnAddrInCallOpcode(addr);
	}
	else
		result |= 1 << 8;


	g_hookAddr_readVehFlags = findPattern("8D 84 24 ? ? ? ? 50 8B CE E8 ? ? ? ? 8B 8C 24 ? ? ? ? 5F 5E 33 CC E8 ? ? ? ? 81 C4 ? ? ? ? C3 ", 0xA);
	if (!g_hookAddr_readVehFlags)
		result |= 1 << 30;

	g_hookAddr_initAutomobileRWheels[0] = findPattern("E8 ? ? ? ? FF 86 ? ? ? ? 0F BF 46 2E 8B 04 85 ? ? ? ? 8B 80 ? ? ? ? 83 78 28 FF ");
	g_hookAddr_initAutomobileRWheels[1] = findPattern("E8 ? ? ? ? FF 86 ? ? ? ? 8B 85 ? ? ? ? 83 78 2C FF ");
	if (!g_hookAddr_initAutomobileRWheels[0] || !g_hookAddr_initAutomobileRWheels[1])
		result |= 1 << 30;

	g_vmtAddr_automobileProcessPhysics = findPattern("C7 06 ? ? ? ? 8D BE ? ? ? ? BE ? ? ? ? 8D 9B ? ? ? ? ", 2);
	if (g_vmtAddr_automobileProcessPhysics) {
		size_t* vmt = *(size_t**)g_vmtAddr_automobileProcessPhysics;

		g_vmtAddr_automobileProcessPhysics = (size_t)(vmt + 0x104 / sizeof size_t);
	}
	else
		result |= 1 << 30;

	//// 
	g_CWeapon__constructor = findPattern("56 8B F1 E8 ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C6 46 24 00");
	if (!g_CWeapon__constructor)
		result |= 1 << 30;

	g_CWeapon__destructor = findPattern("E8 ? ? ? ? 57 E8 ? ? ? ? 83 C4 04 C7 86 ? ? ? ? ? ? ? ? 8D BE ? ? ? ? ");
	if (g_CWeapon__destructor)
		g_CWeapon__destructor = getFnAddrInCallOpcode(g_CWeapon__destructor);
	else
		result |= 1 << 30;

	g_CWeapon__init = findPattern("8B 44 24 04 56 8B F1 B9 ? ? ? ? 89 46 18");
	if (!g_CWeapon__init)
		result |= 1 << 30;

	g_CWeapon__fire = findPattern("55 8B EC 83 E4 F0 83 EC 68 56 8B F1 57 FF 76 18");
	if (!g_CWeapon__fire)
		result |= 1 << 30;

	g_CWeapon__update = findPattern("56 8B F1 8B 46 1C 83 F8 05 0F 87 ? ? ? ? FF 24 85 ? ? ? ? ");
	if (!g_CWeapon__update)
		result |= 1 << 30;

	g_gta_fopen = findPattern("81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 8B 94 24 ? ? ? ? 53 8B 9C 24 ? ? ? ? 32 C9");
	if (!g_gta_fopen)
		result |= 1 << 30;

	g_gta_fclose = findPattern("53 E8 ? ? ? ? 83 C4 04 8B 8C 24 ? ? ? ? 5F 5E 5D 5B 33 CC E8 ? ? ? ? 81 C4 ? ? ? ? C3 ", 1);
	if (g_gta_fclose)
		g_gta_fclose = getFnAddrInCallOpcode(g_gta_fclose);
	else
		result |= 1 << 30;

	g_hookAddr_readIde = findPattern("E8 ? ? ? ? 8B D8 83 C4 08 85 DB 0F 84 ? ? ? ? 8B CE 8D A4 24 ? ? ? ? ");
	if (!g_hookAddr_readIde)
		result |= 1 << 30;

	// Место остановки тут...

	g_getWeaponInfoIdByName = findPattern("E8 ? ? ? ? 83 C4 08 3B F0 0F 85 ? ? ? ? 5F B0 01 ");
	if (g_getWeaponInfoIdByName)
		g_getWeaponInfoIdByName = getFnAddrInCallOpcode(g_getWeaponInfoIdByName);
	else
		result |= 1 << 30;

	g_hookAddr_CVehicle__destructor = findPattern("E8 ? ? ? ? 33 C0 83 C4 08 8B CF 66 89 87 ? ? ? ? E8 ? ? ? ? ");
	if (!g_hookAddr_CVehicle__destructor)
		result |= 1 << 30;

	g_unkObject = (uint32_t**)findPattern("B9 ? ? ? ? E8 ? ? ? ? 8B C8 E8 ? ? ? ? 85 C0 74 30 0F BE 88 ? ? ? ? 49 74 10 ", 1);
	if (g_unkObject)
		g_unkObject = *(uint32_t***)g_unkObject;
	else
		result |= 1 << 30;

	g_unkCall = findPattern("E8 ? ? ? ? 85 C0 74 30 0F BE 88 ? ? ? ? 49 74 10 49 74 03 49 ");
	if (g_unkCall)
		g_unkCall = getFnAddrInCallOpcode(g_unkCall);
	else
		result |= 1 << 30;

	g_unkMatrix34 = (Matrix34*)findPattern("68 ? ? ? ? 8D 4C 24 44 E8 ? ? ? ? 8D 44 24 30 50 8D 4C 24 44 E8 ? ? ? ? F3 0F 10 4C 24 ? ", 1);
	if (g_unkMatrix34)
		g_unkMatrix34 = *(Matrix34**)g_unkMatrix34;
	else
		result |= 1 << 30;

	g_Matrix34__getEuler = findPattern("E8 ? ? ? ? F3 0F 10 4C 24 ? 0F 28 C1 0F 54 05 ? ? ? ? 0F 2F 05 ? ? ? ? 76 31 ");
	if (g_Matrix34__getEuler)
		g_Matrix34__getEuler = getFnAddrInCallOpcode(g_Matrix34__getEuler);
	else
		result |= 1 << 30;

	g_Matrix34__rotateLocalX = findPattern("F3 0F 11 04 24 E8 ? ? ? ? F3 0F 10 64 24 ? F3 0F 10 5C 24 ? F3 0F 10 44 24", 5);
	if (g_Matrix34__rotateLocalX)
		g_Matrix34__rotateLocalX = getFnAddrInCallOpcode(g_Matrix34__rotateLocalX);
	else
		result |= 1 << 30;

	g_CPed__getIsPlayer = getThisCallAddr(&missing_funcs::CPed_CE_getIsPlayer::getIsPlayer);
	if (!g_CPed__getIsPlayer)
		result |= 1 << 30;

	g_vmtAddr_CAutomobile__processCannon = findPattern("C7 06 ? ? ? ? 74 07 C7 46 ? ? ? ? ? 83 BE ? ? ? ? ? BF ? ? ? ? 74 73 ", 2);
	if (g_vmtAddr_CAutomobile__processCannon) {
		size_t* vmt = *(size_t**)g_vmtAddr_CAutomobile__processCannon;
		g_vmtAddr_CAutomobile__processCannon = (size_t)(vmt + 0x1C0 / sizeof size_t);
	}
	else
		result |= 1 << 8;

	g_vmtAddr_CHeli__processCannon = findPattern("C7 06 ? ? ? ? 85 C9 0F 84 ? ? ? ? 83 3D ? ? ? ? ? 7C 56 0F BF 46 2E 3B 05 ? ? ? ? ", 2);
	if (g_vmtAddr_CHeli__processCannon) {
		size_t* vmt = *(size_t**)g_vmtAddr_CHeli__processCannon;
		g_vmtAddr_CHeli__processCannon = (size_t)(vmt + 0x1C0 / sizeof size_t);
	}
	else
		result |= 1 << 8;

	g_vmtAddr_CPlane__processCannon = findPattern("56 FF 74 24 08 8B F1 E8 ? ? ? ? 80 A6 ? ? ? ? ? 81 8E ? ? ? ? ? ? ? ? C7 06 ? ? ? ? C7 86 ? ? ? ? ? ? ? ? ", 0x1D);
	if (g_vmtAddr_CPlane__processCannon) {
		size_t* vmt = *(size_t**)g_vmtAddr_CPlane__processCannon;
		g_vmtAddr_CPlane__processCannon = (size_t)(vmt + 0x1C0 / sizeof size_t);
	}
	else
		result |= 1 << 8;

	g_hookAddr_pedShotBlocking = findPattern("E8 ? ? ? ? 83 C4 04 84 C0 74 51 FF B6 ? ? ? ? 8D 44 24 4C 50 56 ");
	if (!g_hookAddr_pedShotBlocking)
		result |= 1 << 30;


	return result;
}
