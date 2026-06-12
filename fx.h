#pragma once
#include "addrs.h"

struct VehicleFxInfo_s {
	float m_fSkidmarkThresh;
	void* m_pSkidmarkTexDiff;
	void* m_pSkidmarkTexNorm;
	uint8_t m_nbTextureInfoRow;
	uint8_t m_nbTextureInfoCol;
	uint8_t m_nbTextureInfoId;
	float m_fSpecularFalloff;
	float m_fSpecularIntensity;
	uint8_t m_bParallaxSteep;
	float m_bParallaxScale;
	float m_fFrictionThreshMin;
	float m_fFrictionThreshMax;
	float m_fFrictionScaleFactor;
	int32_t m_dwFrictionFxNameHash;
	float m_fDispThreshMin;
	float m_fDispThreshMax;
	float m_fDispScaleFactor;
	uint32_t m_dwDisplacementFxNameHash;
	uint32_t m_dwColnType;
};

enum FxTyreState_e : uint8_t {
	FX_TYRE_STATE_HEALTHY_TIRE_DRY_ROAD = 0,
	FX_TYRE_STATE_WORN_TIRE_DRY_ROAD = 1,
	FX_TYRE_STATE_HEALTHY_TIRE_WET_ROAD = 2,
	FX_TYRE_STATE_WORN_TIRE_WET_ROAD = 3,
};

struct CVehicleFx {
	int32_t _f0;
	int32_t _f4;
	int32_t _f8;
	int32_t m_numInfos;
	VehicleFxInfo_s m_wheelFxInfos[4][42];

	__forceinline VehicleFxInfo_s* getInfo(FxTyreState_e state, int32_t group) { return &m_wheelFxInfos[state][group]; }

};


struct TrailInfo_t {
	int32_t _f0;
	int32_t _f4;
	uint8_t _f8;
	void* _fC;
	int32_t _f10;
	uint8_t __14[0xC];
	Vector4 _f20;
	float _f30;
	float _f34;
	float _f38;
	uint8_t __3C[36];
	Vector3 _f60;
	Vector4 _f70;
	float _f80;
	float _f84;
	uint8_t _f88;
	uint8_t _f89;
	uint8_t _f8A;
	float* _f8C;
	uint8_t __90[8];
	int32_t _f98;
	uint8_t __9C[4];
	float _fA0;
	int32_t _fA4;
	uint8_t __A8[8];
	int32_t _fB0;
	int32_t _fB4;
	int32_t _fB8;
	float _fBC;
	float _fC0;
	int32_t _fC4;
	int32_t _fC8;
	float _fCC;
	float _fD0;
	int32_t _fD4;
	int32_t _fD8;
	float _fDC;
	float _fE0;
	int32_t _fE4;
	int32_t _fE8;
	float _fEC;
};

__forceinline float __cdecl getDistSqrToCamera(Vector3* pVec) {
	return ((float(__cdecl*)(Vector3*))(g_getDistSqrToCamera))(pVec);
}

struct ptxEffectInst {

};

struct CParticleFx {

	// identifier
	ptxEffectInst* registerFxSystem();
};