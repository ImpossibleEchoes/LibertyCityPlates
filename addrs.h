#pragma once
#include "rage_math.h"
#include <inttypes.h>

struct CVehicleModelInfo;
struct CHandlingVehicle;
struct crSkeletonData;

struct grcInstanceData {
	Vector4** m_ppParameters;
	size_t m_pCachedEffect;
	unsigned int m_dwParameterCount;
	unsigned int m_dwEffectSize;
	unsigned char* m_pTypes;
	unsigned int m_dwHash;
	unsigned int _f18;
	unsigned int _f1c;
	unsigned int *m_pParamHash;
};

struct grmShaderFx {
	char __0[0x8];
	char m_nbVersion;
	char m_nbDrawVidro;
	char m_nbFlags;
	char _fb;
	short _fc;
	unsigned short m_wEffectIndex;
	size_t m_pCache;
	grcInstanceData m_instanceData;


};

struct grmShaderGroup {
	char __0[0x8];
	grmShaderFx** m_ppShaders;
	unsigned short m_wCount;

};

struct rmcDrawable {
	char __0[0x8];
	grmShaderGroup *m_pShaderGroup;
	crSkeletonData*m_pSkeletonData;
};

struct CDrawableRef {
	rmcDrawable* pDrawable;
};

struct CVehicleModelInfo {
	char __0[0xc];
	CDrawableRef* m_pDrawableRef; // +C
	char __10[0x2C];
	uint32_t m_hash;
};

extern size_t g_hookAddr_CVehicle__CVehicle; // call opcode
//extern size_t g_atPool__indexOf;
extern size_t* g_pCVehicle__ms_pPool; // указатель на указатель. 
extern size_t g_rage__grmShaderGroup__addShaderGroupVar;
extern size_t g_vmtAddr__CCustomShaderEffectVehicleFX__init;
extern size_t g_vmtAddr__CCustomShaderEffectVehicleFX__update;
extern size_t g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars;
//extern size_t g_hookAddr_createCustomVehFx;
//extern size_t g_hookAddr__CCustomShaderEffectVehicleFX__init;
extern size_t g_CCustomShaderEffectVehicleFX_struct_size;
extern size_t g_rage__grmShaderGroup__SetVarFloat;
extern size_t g_rage__grmShaderGroup__SetVarVector4;
extern size_t g_vmtAddr__CVehicleFactoryNY__createVehicle;
extern size_t g_rand;
extern size_t g_hookAddr_registerNative;

extern size_t g_hookAddr_CVehicle__prerender;
extern size_t g_CDynamicEntity__getSkeletonData;
extern size_t g_CDynamicEntity__getLocalMatrix;
extern size_t g_hookAddr_CVehicle__processControl;
extern float* g_pfTimeStep;

extern size_t g_CPed__getPad2;

extern uint32_t* g_pdwGameTimer;
extern size_t g_isGameKeyboardKeyJustPressed; // only eflc_v1120
extern size_t g_hookAddr_CTransmission__process;
extern size_t g_CAutomobile__processPhysics_engineOnCheckJump;
extern size_t g_hookAddr_CBaseModelInfo__addCustomShaderEffect; // ToDo: delete
extern size_t g_addCustomShaderEffect_vehShadersCheck; // ToDo: delete

extern size_t g_fragInstGta__getFragType;
extern CVehicleModelInfo** g_modelPointers;
extern size_t g_fragType__getChildIndexByBoneIndex;
extern size_t g_vmtAddr_CVehicle__setVehComponentsColFalgs;
extern size_t g_fragType__getGroupIndexByBoneIndex;
extern size_t g_hookAddr_CVehicleModelInfo__setPhysics;
extern CHandlingVehicle* g_handling;

extern size_t g_getDistSqrToCamera;
extern size_t g_ppMaterialMgr;
extern size_t g_pVehFx;
extern size_t g_pUnkVisualEffectObject;
extern size_t g_unkVisualEffectFn1;
extern size_t g_unkVisualEffectFn2;
extern size_t g_hookAddr_CWheel__processSkidmarkVFx;

extern size_t g_CVehicle__setBoneRotation;
extern size_t g_hookAddr_readVehFlags;
extern size_t g_hookAddr_initAutomobileRWheels[2];
extern size_t g_vmtAddr_automobileProcessPhysics;

extern size_t g_CWeapon__constructor;
extern size_t g_CWeapon__destructor;
extern size_t g_CWeapon__init;
extern size_t g_CWeapon__fire;
extern size_t g_CWeapon__update;

extern size_t g_gta_fopen;
extern size_t g_gta_fclose;
extern size_t g_hookAddr_readIde;
extern size_t g_getWeaponInfoIdByName;
extern size_t g_hookAddr_CVehicle__destructor;
extern uint32_t** g_unkObject;
extern size_t g_unkCall;
extern Matrix34* g_unkMatrix34;
extern size_t g_Matrix34__getEuler;
extern size_t g_Matrix34__rotateLocalX;
extern size_t g_CPed__getIsPlayer;
extern size_t g_vmtAddr_CAutomobile__processCannon;
extern size_t g_vmtAddr_CHeli__processCannon;
extern size_t g_vmtAddr_CPlane__processCannon;
extern size_t g_hookAddr_pedShotBlocking;

extern bool g_bIsCE;

uint32_t initAddrsDynamicLegacy();
uint32_t initAddrsDynamicCE();
void initAddrsEFLC1120();