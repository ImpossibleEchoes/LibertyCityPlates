#include "tankComponents.h"

#include "licensePlates.h"
#include "addrs.h"
#include "trace.h"
#include "atPool.h"

#include "helpers.h"
#include "CVehicle.h"
#include "config.h"
#include "newComponents.h"
#include "crSkeleton.h"


void __cdecl updateTankTrack(CCustomShaderEffectVehicleFX*, CVehicle* pVeh, CVehExtParams::Params* pExtParams,
	CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	auto pChars = pExtParams->getPlateChars();
	if (pVars->m_nTrackIndex > 0) {

		auto pTrack = pExtParams->getTankTrackInfo();

		for (size_t i = 0; i < 2; i++)
		pVars->m_fTrack[i] = (pTrack->m_TrackUV[i] + RAGE_PI) * (1 / (RAGE_PI * 2));

	}

}
void __cdecl setVarsTankTrack(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	if (pVars->m_nTrackIndex)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nTrackIndex, pVars->m_fTrack);
}

void __cdecl initTankTrack(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	pVars->m_nTrackIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "uvtrack", false);
}
// ToDo: use flag to determine the side on which the wheel
void __cdecl updateTankTrack(CVehicle* _a, CVehExtParams::Params* pParams) {
	auto numWheels = g_pfnGetVehNumWheels(_a);
	auto pWheels = g_pfnGetVehWheels(_a);

	float val[2]{ 0.f,0.f };

	for (size_t i = 0; i < numWheels; i++) {
		auto pWheel = &pWheels[i];

		switch (pWheel->m_dwBoneId) {
		case 9:
		case 11:
		case 13:
			if (fabsf(pWheel->m_fRpm) > fabsf(val[0]))
				val[0] = pWheel->m_fRpm;
			break;
		case 10:
		case 12:
		case 14:
			if (fabsf(pWheel->m_fRpm) > fabsf(val[1]))
				val[1] = pWheel->m_fRpm;
			break;
		}
	}

	auto pTrack = pParams->getTankTrackInfo();
	

	pTrack->m_TrackUV[0] += val[0] * *g_pfTimeStep;
	pTrack->m_TrackUV[1] += val[1] * *g_pfTimeStep;

	for (size_t i = 0; i < 2; i++) {
		float normalized = fmod(pTrack->m_TrackUV[i] + RAGE_PI, 2.0 * RAGE_PI);
		if (normalized < 0)
			normalized += 2.0 * RAGE_PI;
		pTrack->m_TrackUV[i] = normalized - RAGE_PI;
	}

	pTrack->m_TrackDriveWheelRot[0] += val[0] * *g_pfTimeStep * pTrack->m_TrackDriveWheelScale[0];
	pTrack->m_TrackDriveWheelRot[1] += val[1] * *g_pfTimeStep * pTrack->m_TrackDriveWheelScale[1];

	for (size_t i = 0; i < 2; i++) {
		float normalized = fmod(pTrack->m_TrackDriveWheelRot[i] + RAGE_PI, 2.0 * RAGE_PI);
		if (normalized < 0)
			normalized += 2.0 * RAGE_PI;
		pTrack->m_TrackDriveWheelRot[i] = normalized - RAGE_PI;
	}

}


void __cdecl updateTankComponents(CVehicle* _a, CVehExtParams::Params* pParams) {

}

void __cdecl prerenderTankComponents(CVehicle* _a, CVehExtParams::Params* pParams) {
	auto pTrackInfo = pParams->getTankTrackInfo();
	for (size_t i = COMPONENT_TANK_ROLL_L_FIRST; i <= COMPONENT_TANK_ROLL_L_LAST; i++) {
		int index = pParams->getStruct()->m_aBones[i];
		if (index > 0) {
			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			Quaternion q;
			Vector3 vec{ 1,0,0,0 };
			q.fromAxisAngle(vec, pTrackInfo->m_TrackUV[0]);

			pMtx->fromQuaternion(q);

		}
	}
	for (size_t i = COMPONENT_TANK_ROLL_R_FIRST; i <= COMPONENT_TANK_ROLL_R_LAST; i++) {
		int index = pParams->getStruct()->m_aBones[i];
		if (index > 0) {
			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			Quaternion q;
			Vector3 vec{ 1,0,0,0 };
			q.fromAxisAngle(vec, pTrackInfo->m_TrackUV[1]);

			pMtx->fromQuaternion(q);

		}
	}
	for (size_t i = COMPONENT_TANK_DRIVE_L_FIRST; i <= COMPONENT_TANK_DRIVE_L_LAST; i++) {
		int index = pParams->getStruct()->m_aBones[i];
		if (index > 0) {
			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			Quaternion q;
			Vector3 vec{ 1,0,0,0 };
			q.fromAxisAngle(vec, pTrackInfo->m_TrackDriveWheelRot[0]);

			pMtx->fromQuaternion(q);

		}
	}
	for (size_t i = COMPONENT_TANK_DRIVE_R_FIRST; i <= COMPONENT_TANK_DRIVE_R_LAST; i++) {
		int index = pParams->getStruct()->m_aBones[i];
		if (index > 0) {
			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			Quaternion q;
			Vector3 vec{ 1,0,0,0 };
			q.fromAxisAngle(vec, pTrackInfo->m_TrackDriveWheelRot[1]);

			pMtx->fromQuaternion(q);

		}
	}

}

void initTankComponents() {

	if (CConfig::ms_bTankComponents) {
		if (CConfig::ms_bPatchCustomVehFX) {
			g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnInit = initTankTrack;
			g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnSetVars = setVarsTankTrack;
			g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnUpdate = updateTankTrack;
			g_numCustomVehFxAddons++;
			PRINT_DUBUG("[initTankComponents(...)] added uvtrack\n");
		}
		g_pfnVehProcessControlAddon[g_numVehProcessControlAddon++] = updateTankTrack;
		g_pfnVehPrerenderAddon[g_numVehPrerenderAddon++] = prerenderTankComponents;

	}


}