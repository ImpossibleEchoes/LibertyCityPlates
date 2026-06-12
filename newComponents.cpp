#include "newComponents.h"

#include "crSkeleton.h"
#include "addrs.h"
#include "CVehicle.h"
#include "utils.h"
#include "hookFns.h"
#include "config.h"
#include "CHandling.h"

#include "helpers.h"

void(__cdecl* g_pfnVehProcessControlAddon[0x10])(CVehicle*, CVehExtParams::Params*);
uint8_t g_numVehProcessControlAddon = 0;

void(__cdecl* g_pfnVehPrerenderAddon[0x10])(CVehicle*, CVehExtParams::Params*);
uint8_t g_numVehPrerenderAddon = 0;




std::map<uint16_t, CVehStruct2*> g_vehStruct2;

struct CVehicle2 : CVehicle {

	static size_t ms_prerender_origcall;
	static size_t ms_processcontrol_origcall;

	uint8_t run_prerender() {
		auto ret = ((uint8_t(__thiscall*)(CVehicle * _a))(ms_prerender_origcall))(this);
		
		uint32_t type1 = g_pfnGetVehMainType(this);
		uint32_t type2 = g_pfnGetVehSpecialType(this);
		if ((type1 == 0 && type2 == 0) ||
			(type1 == 1 && type2 == 1)) {

			auto pParams = g_extVehParams.getFromVehicle(this);

			if (pParams) {
				for (size_t i = 0; i < g_numVehPrerenderAddon; i++)
					g_pfnVehPrerenderAddon[i](this, pParams);
			}
		}
		return ret;
	}

	void run_processcontrol() {
		((void(__thiscall*)(CVehicle * _a))(ms_processcontrol_origcall))(this);
		
		CVehicle* pVeh = *reinterpret_cast<CVehicle**>(reinterpret_cast<uint8_t*>(this) - 0x10);

		uint32_t type1 = g_pfnGetVehMainType(pVeh);
		uint32_t type2 = g_pfnGetVehSpecialType(pVeh);
		if ((type1 == 0 && type2 == 0) ||
			(type1 == 1 && type2 == 1)) {

			auto pParams = g_extVehParams.getFromVehicle(pVeh);

			if (pParams) {
				for (size_t i = 0; i < g_numVehProcessControlAddon; i++)
					g_pfnVehProcessControlAddon[i](pVeh, pParams);
			}
		}
	}

};
size_t CVehicle2::ms_prerender_origcall;
size_t CVehicle2::ms_processcontrol_origcall;

//uint32_t __stdcall hook_CVehicle__getTruckExhaustFxFlag(CVehicle* _a) {
//
//	auto pParams = g_extVehParams.getFromVehicle(_a);
//
//	if (pParams) {
//		for (size_t i = 0; i < g_numVehPrerenderAddon; i++)
//			g_pfnVehPrerenderAddon[i](_a, pParams);
//	}
//
//	return ((uint32_t(__stdcall*)(CVehicle * _a))(CVehicle__prerender_origcall))(_a);
//}

void __cdecl checkVehStruct2(CVehicle* _a, CVehExtParams::Params* pParams, crSkeletonData* pSkel, uint32_t modelId) {

	if (!g_vehStruct2.count((modelId != ~0) ? modelId : _a->getModelIndex())) {

		CVehStruct2* pStruct = new CVehStruct2;

		const char* ppszNames[NUM_COMPONENTS]{
			"steer_mesh",
			"steeringwheel",
			"speed_mesh",
			"dial_speed",
			"temp_mesh",
			"dial_temp",
			"rpm_mesh",
			"dial_rpm",
			"engine_mesh",
			"engineblock",
			"supercharger_throttle_mesh",
			"supercharger_1",
			"supercharger_2",
			"supercharger_3",
			"popup_headlights",
			"platelight",
			"platelight_2",
			"platelight_3",
			"platelight_4",

			"exhaust",
			"exhaust_2",
			"exhaust_3",
			"exhaust_4",
			"exhaust_5",
			"exhaust_6",
			"exhaust_7",
			"exhaust_8",
			"exhaust_9",
			"exhaust_10",
			"exhaust_11",
			"exhaust_12",
			"exhaust_13",
			"exhaust_14",
			"exhaust_15",
			"exhaust_16",

			"roll_l_1",
			"roll_l_2",
			"roll_l_3",
			"roll_l_4",
			"roll_l_5",

			"roll_r_1",
			"roll_r_2",
			"roll_r_3",
			"roll_r_4",
			"roll_r_5",

			"drive_l_1",
			"drive_l_2",

			"drive_r_1",
			"drive_r_2",

			"weapon_a",
			"weapon_b",
			"weapon_c",
			"weapon_d",
			"weapon_e",
			"weapon_f",
			"weapon_g",
			"weapon_h",
			"weapon_i",
			"weapon_j",

			"turret_1base",
			"turret_1barrel",

			"turret_2base",
			"turret_2barrel",
		};

		FillMemory(pStruct->m_aBones, sizeof pStruct->m_aBones, 0xFF);

		for (size_t i = 0; i < sizeof ppszNames / sizeof * ppszNames; i++) {
			for (size_t j = 0; j < pSkel->m_wNumBones; j++) {
				if (!strcmp(pSkel->m_pBones[j].pszName, ppszNames[i])) {
					pStruct->m_aBones[i] = j;
					break;
				}
			}
		}

		if (CConfig::ms_bTankComponents) {
			if (pStruct->m_aBones[COMPONENT_TANK_DRIVE_L_FIRST] != -1)
				pStruct->m_TrackDriveWheelScale[0] = pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TANK_DRIVE_L_FIRST]].m_vRotMin.x;
			else
				pStruct->m_TrackDriveWheelScale[0] = 0.f;
			if (pStruct->m_aBones[COMPONENT_TANK_DRIVE_R_FIRST] != -1)
				pStruct->m_TrackDriveWheelScale[1] = pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TANK_DRIVE_R_FIRST]].m_vRotMin.x;
			else
				pStruct->m_TrackDriveWheelScale[1] = 0.f;
		}

		pParams->setStruct(pStruct);

		g_vehStruct2.insert(std::make_pair((modelId != ~0) ? modelId : _a->getModelIndex(), pStruct));
	}
	else {
		auto p = g_vehStruct2[(modelId != ~0) ? modelId : _a->getModelIndex()];
		pParams->setStruct(p);
	}

	if (CConfig::ms_bTankComponents) {
		auto pStruck = pParams->getStruct();
		auto pTrack = pParams->getTankTrackInfo();
		pTrack->m_TrackDriveWheelScale[0] = pStruck->m_TrackDriveWheelScale[0];
		pTrack->m_TrackDriveWheelScale[1] = pStruck->m_TrackDriveWheelScale[1];
	}
}

void __cdecl checkVehStruct2(CVehicle* _a, CVehExtParams::Params* pParams, uint32_t modelId) {
	auto pSkel = g_modelPointers[modelId]->m_pDrawableRef->pDrawable->m_pSkeletonData;
	checkVehStruct2(_a, pParams, pSkel, modelId);
}

void __cdecl checkVehStruct2(CVehicle* _a, CVehExtParams::Params* pParams) {
	auto pSkel = ((crSkeletonData * (__thiscall*)(CVehicle * _a))(g_CDynamicEntity__getSkeletonData))(_a);
	checkVehStruct2(_a, pParams, pSkel, ~0);
}

//
//int g_components[3]{
//	COMPONENT_RPM,
//	COMPONENT_SPEED,
//	COMPONENT_TEMP
//};

const float DASHBOARD_SMOOTH_TIME = 0.2f;

void __cdecl updateDashboard(CVehicle* _a, CVehExtParams::Params* pParams) {

	char flags = g_pfnGetVehFlags1_0(_a);
	if (flags & 8) { // engine on


		float speed = 0;

		uint32_t numWheels = g_pfnGetVehNumWheels(_a);
		CWheel* pWheels = g_pfnGetVehWheels(_a);

		for (size_t i = 0; i < numWheels; i++)
			if (pWheels[i].m_bDriveWheel)
				speed = max(speed, fabsf(pWheels[i].m_fRpm) * pWheels[i].m_fWheelRadius * 2 * RAGE_PI /*addon*/ * 0.16f * 2.4375f);

		float old = pParams->getSpeedArrow();
		pParams->setSpeedArrow(smoothFloat(speed, old, *(float*)g_pfTimeStep, DASHBOARD_SMOOTH_TIME));

		speed = g_pfnGetVehRpm(_a);

		old = pParams->getRpmArrow();
		pParams->setRpmArrow(smoothFloat(speed, old, *(float*)g_pfTimeStep, DASHBOARD_SMOOTH_TIME));

		speed = g_pfnGetVehTemp(_a);
		old = pParams->getTempArrow();
		pParams->setTempArrow(smoothFloat(speed, old, *(float*)g_pfTimeStep, DASHBOARD_SMOOTH_TIME));
	}
	else {
		float old = pParams->getSpeedArrow();
		if(old != 0.f)
			pParams->setSpeedArrow(smoothFloat(0, old, *(float*)g_pfTimeStep, DASHBOARD_SMOOTH_TIME));
		old = pParams->getRpmArrow();
		if(old != 0.f)
			pParams->setRpmArrow(smoothFloat(0, old, *(float*)g_pfTimeStep, DASHBOARD_SMOOTH_TIME));
		old = pParams->getTempArrow();
		if(old != 0.f)
			pParams->setTempArrow(smoothFloat(0, old, *(float*)g_pfTimeStep, DASHBOARD_SMOOTH_TIME));
	}

}


void __cdecl prerenderDashboard(CVehicle* _a, CVehExtParams::Params* pParams) {
	auto pStruct = pParams->getStruct();
	auto pSkel = ((crSkeletonData * (__thiscall*)(CVehicle * _a))(g_CDynamicEntity__getSkeletonData))(_a);

	for (size_t i = COMPONENT_RPM_FIRST; i <= COMPONENT_RPM_LAST; i++) {
		if (pStruct->m_aBones[i] != -1) {
			auto index = pStruct->m_aBones[i];
			auto pBone = pSkel->m_pBones + index;


			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			float val = pParams->getRpmArrow();

			float rot = val;
			rot *= pBone->m_vRotMin.z;
			if (pBone->m_vRotMin.x != 0.f) {
				rot -= pBone->m_vRotMin.x;
				rot = max(rot, 0);
			}
			rot = min(pBone->m_vRotMin.y, rot);

			Quaternion quat;
			Vector3 axisVec{ 0,1,0,0 };
			quat.fromAxisAngle(axisVec, rot);

			Quaternion quat2;
			quat2 = pSkel->m_pBones[index].m_vRotationQuaternion;

			Quaternion quat3 = quat2 * quat;

			pMtx->fromQuaternion(quat3);

		}
	}

	for (size_t i = COMPONENT_SPEED_FIRST; i <= COMPONENT_SPEED_LAST; i++) {
		if (pStruct->m_aBones[i] != -1) {
			auto index = pStruct->m_aBones[i];
			auto pBone = pSkel->m_pBones + index;


			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			float val = pParams->getSpeedArrow();

			float rot = val;
			rot *= pBone->m_vRotMin.z;
			if (pBone->m_vRotMin.x != 0.f) {
				rot -= pBone->m_vRotMin.x;
				rot = max(rot, 0);
			}
			rot = min(pBone->m_vRotMin.y, rot);

			Quaternion quat;
			Vector3 axisVec{ 0,1,0,0 };
			quat.fromAxisAngle(axisVec, rot);

			Quaternion quat2;
			quat2 = pSkel->m_pBones[index].m_vRotationQuaternion;

			Quaternion quat3 = quat2 * quat;

			pMtx->fromQuaternion(quat3);

		}
	}

	for (size_t i = COMPONENT_TEMP_FIRST; i <= COMPONENT_TEMP_LAST; i++) {
		if (pStruct->m_aBones[i] != -1) {
			auto index = pStruct->m_aBones[i];
			auto pBone = pSkel->m_pBones + index;


			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			float val = pParams->getTempArrow();

			float rot = val;
			rot *= pBone->m_vRotMin.z;
			if (pBone->m_vRotMin.x != 0.f) {
				rot -= pBone->m_vRotMin.x;
				rot = max(rot, 0);
			}
			rot = min(pBone->m_vRotMin.y, rot);

			Quaternion quat;
			Vector3 axisVec{ 0,1,0,0 };
			quat.fromAxisAngle(axisVec, rot);

			Quaternion quat2;
			quat2 = pSkel->m_pBones[index].m_vRotationQuaternion;

			Quaternion quat3 = quat2 * quat;

			pMtx->fromQuaternion(quat3);

		}
	}

	for (size_t i = COMPONENT_STEER_FIRST; i <= COMPONENT_STEER_LAST; i++) {
		if (pStruct->m_aBones[i] != -1) {
			auto index = pStruct->m_aBones[i];
			auto pBone = pSkel->m_pBones + index;


			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			uint32_t numWheels = g_pfnGetVehNumWheels(_a);
			CWheel* pWheels = g_pfnGetVehWheels(_a);

			float val = 0;
			if (numWheels != 0) {
				for (uint8_t i = 0; i < numWheels; i++) {
					CWheel* wheel = pWheels + i;
					if (wheel->m_bFrontWheelSteer)
						val = wheel->m_fRotationZ > 0.f ? -wheel->m_fRotationZ : -wheel->m_fRotationZ * 1.33333334f;
					else if (wheel->m_bRearWheelSteer)
						val = wheel->m_fRotationZ < 0.f ? wheel->m_fRotationZ : wheel->m_fRotationZ * 1.33333334f;
					else continue;
					auto pHandling = g_pfnGetHandlingData(_a);

					/*

					Seriously?! i can't believe that bugged line was active for almost two years... \
					it's been in five different scripts 'cuz i just kept grabbing the core steering feature code and pasting it everywhere. Ugh

					wheel->m_fRotationZ /= pHandling->m_fSteeringLock;

					*/

					val /= pHandling->m_fSteeringLock;
					break;
				}
			}


			float rot = val * pBone->m_vRotMin.x;

			Quaternion quat;
			Vector3 axisVec{ 0,1,0,0 };
			quat.fromAxisAngle(axisVec, rot);

			Quaternion quat2;
			quat2 = pSkel->m_pBones[index].m_vRotationQuaternion;

			Quaternion quat3 = quat2 * quat;

			pMtx->fromQuaternion(quat3);

		}
	}


}

// попы

void __cdecl prerenderПопов(CVehicle* _a, CVehExtParams::Params* pParams) {
	auto pStruct = pParams->getStruct();
	auto pSkel = ((crSkeletonData * (__thiscall*)(CVehicle * _a))(g_CDynamicEntity__getSkeletonData))(_a);
	if (pStruct->m_aBones[COMPONENT_POPUP_HEADLIGHTS] != -1) {
		auto index = pStruct->m_aBones[COMPONENT_POPUP_HEADLIGHTS];
		auto pBone = pSkel->m_pBones + index;


		auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

		float val = 0;

		bool b1 = *g_pdwGameTimer < g_pfnGetUnkTimer(_a) && (*g_pdwGameTimer & 0x100) != 0;
		bool b2 = (g_pfnGetVehicleFlags2_1(_a) & 2) != 0 || b1;
		if ((g_pfnGetVehicleFlags1_1(_a) & 1) != 0)
			b2 = true;
		if ((g_pfnGetForceCarLightMask(_a) & 3) == 2 || b2)
			b1 = true;
		else
			b1 = false;


		float rot;
		if (b1)
			rot = pBone->m_vRotMax.x;
		else
			rot = 0.f;

		Quaternion quat;
		Vector3 axisVec{ 1,0,0,0 };
		quat.fromAxisAngle(axisVec, rot);

		Quaternion quat2;
		quat2 = pSkel->m_pBones[index].m_vRotationQuaternion;

		Quaternion quat3 = quat2 * quat;

		pMtx->fromQuaternion(quat3);


	}
}


// engine

void __cdecl updateEngineComponents(CVehicle* _a, CVehExtParams::Params* pParams) {
	float rpm = g_pfnGetVehRpm(_a);

	float rot = pParams->getEngineRot();
	rot += rpm * *(float*)g_pfTimeStep * 50 * 4;
	while (rot > RAGE_PI)
		rot -= RAGE_PI * 2;
	pParams->setEngineRot(rot);

	float old = pParams->getSuperchargerThrottleRot();
	rot = g_pfnGetVehThrottle(_a);
	rot = rot < 0.f ? -rot : rot;
	if (rot > 0.9f)
		rot = 0.9f;
	rot *= 1.11111111111f;
	pParams->setSuperchargerThrottleRot(smoothFloat(rot, old, *(float*)g_pfTimeStep, 0.2f));

}

void __cdecl prerenderEngineComponents(CVehicle* _a, CVehExtParams::Params* pParams) {
	auto pStruct = pParams->getStruct();
	auto pSkel = ((crSkeletonData * (__thiscall*)(CVehicle * _a))(g_CDynamicEntity__getSkeletonData))(_a);
	for (size_t i = COMPONENT_ENGINE_FIRST; i <= COMPONENT_ENGINE_LAST; i++) {
		if (pStruct->m_aBones[i] != -1) {
			auto index = pStruct->m_aBones[i];
			auto pBone = pSkel->m_pBones + index;

			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			float rot = pParams->getEngineRot();

			rot += RAGE_PI;
			rot *= 0.5f;

			rot = sinf(rot);

			float rpm = g_pfnGetVehRpm(_a);
			rot *= rpm * rpm;

			rot *= pBone->m_vRotMin.x;
			Quaternion q;
			Vector3 vec{ 0,1,0,0 };
			q.fromAxisAngle(vec, rot);

			//Quaternion quat2;
			////quat2 = pBone->m_vRotationQuaternion;
			//quat2.fromMatrix(pMtx);

			//Quaternion quat3 = quat2 * q;

			pMtx->fromQuaternion(q);
		}
	}

	for (size_t i = COMPONENT_SUPERCHARGER_THROTTLE_FIRST; i <= COMPONENT_SUPERCHARGER_THROTTLE_LAST; i++) {
		if (pStruct->m_aBones[i] != -1) {
			auto index = pStruct->m_aBones[i];
			auto pBone = pSkel->m_pBones + index;

			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(_a, index);

			float rot = pParams->getSuperchargerThrottleRot();

			rot *= pBone->m_vRotMin.x;
			Quaternion q;
			Vector3 vec{ 1,0,0,0 };
			q.fromAxisAngle(vec, rot);

			//Quaternion quat2;
			//quat2 = pBone->m_vRotationQuaternion;

			//Quaternion quat3 = quat2 * q;

			pMtx->fromQuaternion(q);
		}
	}


}

void initNewComponents() {

	if (CConfig::ms_bPatchEngine) {
		if (CConfig::ms_bEngineComponents) {
			g_pfnVehPrerenderAddon[g_numVehPrerenderAddon++] = prerenderEngineComponents;
			g_pfnVehProcessControlAddon[g_numVehProcessControlAddon++] = updateEngineComponents;
		}
	}

	if (CConfig::ms_bDashboard) {

		g_pfnVehPrerenderAddon[g_numVehPrerenderAddon++] = prerenderDashboard;

		g_pfnVehProcessControlAddon[g_numVehProcessControlAddon++] = updateDashboard;
	}
	
	if(CConfig::ms_bPopupHeadlights)
		g_pfnVehProcessControlAddon[g_numVehProcessControlAddon++] = prerenderПопов;

	CVehicle2::ms_prerender_origcall = setFnAddrInCallOpcode(g_hookAddr_CVehicle__prerender, getThisCallAddr(&CVehicle2::run_prerender));
	CVehicle2::ms_processcontrol_origcall = setFnAddrInCallOpcode(g_hookAddr_CVehicle__processControl, getThisCallAddr(&CVehicle2::run_processcontrol));

	PRINT_DUBUG("[initNewComponents(...)] %u hooks initialized in CAutomobile::prerender\n", g_numVehPrerenderAddon);
	PRINT_DUBUG("[initNewComponents(...)] %u hooks initialized in CAutomobile::processControl\n", g_numVehProcessControlAddon);

}
