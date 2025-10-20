#include "newVehFlags.h"

#include "addrs.h"
#include "hookFns.h"
#include "trace.h"
#include "CVehicle.h"
#include "vehExtParams.h"
#include "helpers.h"
#include "newComponents.h"
#include "utils.h"

void(__cdecl* g_pfnVehProcessPhysicsAddon[0x10])(CVehicle*, CVehExtParams::Params*);
uint8_t g_numVehProcessPhysicsAddon = 0;


std::map<CVehicleModelInfo*, CVehNewFlags::Flags> CVehNewFlags::ms_flags;

size_t CVehNewFlags::ms_readFlags_origcall;

struct CVehicle;

void* CVehNewFlags::runHook(const char* flags) {
	auto retVal = ((void* (__thiscall*)(void*, const char*))(ms_readFlags_origcall))(this, flags);

	auto newFlags = getNewVehFlags(reinterpret_cast<CVehicleModelInfo*>(this));
	if (!newFlags) {
		auto _flags = CVehNewFlags::Flags();
		ms_flags.insert(std::make_pair(reinterpret_cast<CVehicleModelInfo*>(this), _flags));
		newFlags = getNewVehFlags(reinterpret_cast<CVehicleModelInfo*>(this));
	}

	auto v3 = flags;
	char v6[128];
	int i = 0;
	while (*v3) {
		for (; *v3 == '+'; ++v3)
			;
		auto v4 = *v3;
		for (i = 0; *v3; ++i) {
			if (v4 == '+')
				break;
			++v3;
			v6[i] = v4;
			v4 = *v3;
		}
		v6[i] = 0;

		if (!strcmp("4wsteer", v6)) {
			newFlags->m_flags |= NEWVEHFLAG_STEER_FR_WHEELS;
			PRINT_DUBUG("front and rear wheels steer\n");
		}
		else if (!strcmp("tankfx", v6)) {
			newFlags->m_flags |= NEWVEHFLAG_SKIDMARK_ONLY_FROM_LAST_WHEEL_BY_VECTOR;
			PRINT_DUBUG("tank skidmark\n");
		}

	}


	return retVal;
}

struct CWheel_newFlags : CWheel {

	static size_t ms_init_origcall;

	uint32_t initWheel(CVehicle* pVeh, int boneId, float fDefaultWheelRadius, float fTyreStability, uint32_t flags) {

		auto pModelInfo = g_modelPointers[pVeh->getModelIndex()];

		if (auto pNewFlags = CVehNewFlags::getNewVehFlags(pModelInfo)) {
			if (pNewFlags->m_flags & NEWVEHFLAG_STEER_FR_WHEELS)
				flags |= 0x4000;
		}

		return ((uint32_t(__thiscall*)(CWheel*, CVehicle *, int, float, float, uint32_t))(ms_init_origcall))(this, pVeh, boneId, fDefaultWheelRadius, fTyreStability, flags);
	}
};

size_t CWheel_newFlags::ms_init_origcall;

void __cdecl setNewFlags(CVehicle* _a, CVehExtParams::Params* pParams) {
	auto pModelInfo = g_modelPointers[_a->getModelIndex()];
	if (auto pNewFlags = CVehNewFlags::getNewVehFlags(pModelInfo)) {
		if (pNewFlags->m_flags & NEWVEHFLAG_SKIDMARK_ONLY_FROM_LAST_WHEEL_BY_VECTOR)
			pParams->setUseTankSkidmark(true);

	}
}

void __cdecl processTankSkidmark(CVehicle* _a, CVehExtParams::Params* pParams) {
	if (pParams->getUseTankSkidmark()) {
		auto pTransform = _a->getTransform();
		//Vector3 velocity; _a->getVelocity(&velocity);

		float forwardSpeed;// = sqrtf(velocity.x * pTransform->b.x) + (velocity.y * pTransform->b.y) + (velocity.z * pTransform->b.z);
		auto numWheels = g_pfnGetVehNumWheels(_a);
		auto ppWheels = g_pfnGetVehWheels(_a);

		forwardSpeed = 0;
		for (size_t i = 0; i < numWheels; i++) {
			auto pWheel = ppWheels + i;
			if (pWheel->m_bDriveWheel)
				forwardSpeed += -pWheel->m_fRpm;
		}

		if (forwardSpeed >= 0.f) {
			for (size_t i = 0; i < numWheels; i++) {
				auto pWheel = ppWheels + i;
				switch (pWheel->m_dwBoneId) {
				case 0xD: // LR
				case 0xE: // RR
					if (pWheel->m_dwFlags & 0x10000000)
						pWheel->m_fSkidmarkIntensity = 1.f;
					//if (pWheel->m_fSidemarkIntensity > 0.3f)
					//	pWheel->m_fSidemarkIntensity = 0.3f;
					break;
				default:
					pWheel->m_fSkidmarkIntensity = 0.f;
					break;
				}
			}
		}
		else {
			for (size_t i = 0; i < numWheels; i++) {
				auto pWheel = ppWheels + i;
				switch (pWheel->m_dwBoneId) {
				case 0x9: // LF
				case 0xA: // RF
					if (pWheel->m_dwFlags & 0x10000000)
						pWheel->m_fSkidmarkIntensity = 1.f;
					//if (pWheel->m_fSidemarkIntensity > 0.3f)
					//	pWheel->m_fSidemarkIntensity = 0.3f;
					break;
				default:
					pWheel->m_fSkidmarkIntensity = 0.f;
					break;
				}
			}
		}
	}

}

struct CAutomobilePrococessPhysicsHook : CVehicle {

	static size_t ms_processPhysics_origcall;

	int processPhysics(float fTimeStep, char bCanPostpone, int nTimeSlice) {
		auto retVal = ((int(__thiscall*)(CVehicle*, float, char, int))(ms_processPhysics_origcall))(this, fTimeStep, bCanPostpone, nTimeSlice);

		if (auto pParams = g_extVehParams.getFromVehicle(this))
			for (size_t i = 0; i < g_numVehProcessPhysicsAddon; i++)
				g_pfnVehProcessPhysicsAddon[i](this, pParams);


		return retVal;
	}

	static void init() {
		ms_processPhysics_origcall = writeDWORD(g_vmtAddr_automobileProcessPhysics, getThisCallAddr(&processPhysics));
	}
};

size_t CAutomobilePrococessPhysicsHook::ms_processPhysics_origcall;


void CVehNewFlags::init() {
	//setFnAddrInCallOpcode(g_hookAddr_processCannonOrientation, getThisCallAddr(&veh::processCannonOrientation));


	ms_readFlags_origcall = setFnAddrInCallOpcode(g_hookAddr_readVehFlags, getThisCallAddr(&runHook));
	CWheel_newFlags::ms_init_origcall = setFnAddrInCallOpcode(g_hookAddr_initAutomobileRWheels[0], getThisCallAddr(&CWheel_newFlags::initWheel));
	setFnAddrInCallOpcode(g_hookAddr_initAutomobileRWheels[1], getThisCallAddr(&CWheel_newFlags::initWheel));

	CAutomobilePrococessPhysicsHook::init();

	g_pfnVehFactoryAddon[g_numVehFactoryAddon++] = setNewFlags;
	g_pfnVehProcessPhysicsAddon[g_numVehProcessPhysicsAddon++] = processTankSkidmark;

}