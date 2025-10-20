#include "vehExtParams.h"

#include "hookFns.h"
#include "addrs.h"

#include <stdio.h>
#include <utility> 
#include <map>

#include "trace.h"
#include "config.h"
#include "licensePlates.h"
#include "newComponents.h"
#include "atPool.h"
#include "CVehicle.h"
#include "utils.h"
#include "helpers.h"

CVehExtParams g_extVehParams;

size_t CVehExtParams::Params::ms_charsOffset = -1;
size_t CVehExtParams::Params::ms_structOffset = -1;
size_t CVehExtParams::Params::ms_size = 0;
size_t CVehExtParams::Params::ms_speedArrowOffset = -1;
size_t CVehExtParams::Params::ms_rpmArrowOffset = -1;
size_t CVehExtParams::Params::ms_tempArrowOffset = -1;
size_t CVehExtParams::Params::ms_engineRotOffset = -1;
size_t CVehExtParams::Params::ms_superchargerThrottleRotOffset = -1;
size_t CVehExtParams::Params::ms_tankTrackParamsOffset = -1;
size_t CVehExtParams::Params::ms_moreWheelsOffset = -1;
size_t CVehExtParams::Params::ms_useTankSkidmarkOffset = -1;
size_t CVehExtParams::Params::ms_vehWeapOffset = -1;


// массив для них
void(__cdecl* g_pfnVehFactoryAddon[0x10])(CVehicle*, CVehExtParams::Params*);
uint8_t g_numVehFactoryAddon = 0;

uint32_t g_modelId = 0xFFFFFFFF;

CVehicle* CVehicleFactory::createVehicle(uint32_t dwModel, uint32_t createdBy, void* pMtx, BOOL bNetwork) {
	g_modelId = dwModel;
	auto pVeh = ((CVehicle * (__thiscall*)(CVehicleFactory*, uint32_t, uint32_t, void*, BOOL))ms_createVehicle_origcall)(this, dwModel, createdBy, pMtx, bNetwork);

	if (pVeh) {
		{
			auto pVehPool = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool));
			//auto index = pVehPool->indexOf(pVeh);
			//if (index >= g_extVehParams.m_size)
			//	g_extVehParams.resize(index + 1);
			
			auto size = pVehPool->m_dwSize;
			if (size != g_extVehParams.m_size) {
				g_extVehParams.resizeNoAlign(size);

				g_extVehParams.m_pVehMemStart = (CVehicle*)pVehPool->m_pObjects;
				g_extVehParams.m_pVehMemEnd = (CVehicle*)(pVehPool->m_pObjects + pVehPool->m_dwSize * pVehPool->m_dwEntrySize);
			}
		}

		if (auto pParams = g_extVehParams.getFromVehicle(pVeh)) {
			for (size_t i = 0; i < g_numVehFactoryAddon; i++)
				g_pfnVehFactoryAddon[i](pVeh, pParams);

		}
	}
	return pVeh;
}

size_t g_destroyVehicle_origcall;
void __cdecl destroyVehicle_hook(CVehicle* a1) {
	((void (__cdecl*)(CVehicle*))g_destroyVehicle_origcall)(a1);

	auto pExt = g_extVehParams.getFromVehicle(a1);
	pExt->destroy();
}

void CVehicleFactory::init() {

	//g_pfnVehFactoryAddon[g_numVehFactoryAddon++] = checkVehStruct2;
	if(CConfig::ms_bLicensePlates)
		g_pfnVehFactoryAddon[g_numVehFactoryAddon++] = setStandartLicensePlate;

	if(g_numVehFactoryAddon)
		ms_createVehicle_origcall = writeDWORD(g_vmtAddr__CVehicleFactoryNY__createVehicle, getThisCallAddr(&createVehicle));

	g_destroyVehicle_origcall = setFnAddrInCallOpcode(g_hookAddr_CVehicle__destructor, (size_t)destroyVehicle_hook);
}

size_t CVehicleFactory::ms_createVehicle_origcall = 0;

// хуки для транспорта
struct tVeh : CVehicle {

	static size_t ms_constructor;

	void CVehicle__CVehicle() {

		if (auto pParams = g_extVehParams.getFromVehicle(this)) {
			pParams->setDefaultParams();
			
		}
		else {
			auto pVehPool = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool));
			//auto index = pVehPool->indexOf(pVeh);
			//if (index >= g_extVehParams.m_size)
			//	g_extVehParams.resize(index + 1);

			auto size = pVehPool->m_dwSize;
			if (size != g_extVehParams.m_size) {
				g_extVehParams.resizeNoAlign(size);

				g_extVehParams.m_pVehMemStart = reinterpret_cast<CVehicle*>(pVehPool->m_pObjects);
				g_extVehParams.m_pVehMemEnd = reinterpret_cast<CVehicle*>(pVehPool->m_pObjects + pVehPool->m_dwSize * pVehPool->m_dwEntrySize);
			}

			if (auto pParams = g_extVehParams.getFromVehicle(this)) {
				pParams->setDefaultParams();
				PRINT_DUBUG("[CVehExtParams::getFromVehicle(...)] fixed entry %p\n", this);
			}

		}

		if (g_modelId != 0xFFFFFFFF) {
			auto pParams = g_extVehParams.getFromVehicle(this);
			checkVehStruct2(this, pParams, g_modelId);
			g_modelId = 0xFFFFFFFF;
		}
		else {
			MessageBoxA(nullptr, "invalid CVehicle::CVehicle call", nullptr, 0x10);
		}


		((void(__thiscall*)(tVeh*))(ms_constructor))(this);
	}

};
size_t tVeh::ms_constructor;

bool CVehExtParams::resize(size_t newSize) {
	size_t oldSize = m_size;

	m_size = (newSize + ms_alignas - 1) & ~(ms_alignas - 1);

	Params* pNewParams = reinterpret_cast<Params*>(malloc(m_size * Params::ms_size));
	if (oldSize) {
		memcpy(pNewParams, m_pParams, min(Params::ms_size * oldSize, Params::ms_size * m_size));
		free(m_pParams);
	}
	m_pParams = pNewParams;

	PRINT_DUBUG("[CVehExtParams::resize(...)] resized from %u to %u\n", oldSize, m_size);

	return true;
}

bool CVehExtParams::resizeNoAlign(size_t newSize) {
	size_t oldSize = m_size;

	m_size = newSize;

	Params* pNewParams = reinterpret_cast<Params*>(malloc(m_size * Params::ms_size));
	if (oldSize) {
		memcpy(pNewParams, m_pParams, min(Params::ms_size * oldSize, Params::ms_size * m_size));
		free(m_pParams);
	}
	m_pParams = pNewParams;

	PRINT_DUBUG("[CVehExtParams::resizeNoAlign(...)] resized from %u to %u\n", oldSize, m_size);

	return true;
}

CVehExtParams::Params* CVehExtParams::getByIndex(int index) noexcept {
	return reinterpret_cast<Params*>(reinterpret_cast<uint8_t*>(m_pParams) + Params::ms_size * index);
}

CVehExtParams::Params* CVehExtParams::getFromVehicle(CVehicle* veh) noexcept {

	if (veh >= m_pVehMemStart && veh < m_pVehMemEnd) {
		uint32_t index = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOf(veh);
		return reinterpret_cast<Params*>(reinterpret_cast<uint8_t*>(m_pParams) + Params::ms_size * index);
	}
	//else if (veh >= m_pVehMemStart && veh < m_pVehMemEnd) {

	//}
	else {



#ifdef _DEBUG
		uint32_t index = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOf(veh);
		PRINT_DUBUG("[CVehExtParams::getFromVehicle(...)] bad entry %p. Out of bounds. Index = %i\n", veh, index);
		PRINT_DUBUG("[CVehExtParams::getFromVehicle(...)] real vmt = 0x%08x, vmt in ida db(base = 0x400000) 0x%08x\n", *(size_t*)veh, *(size_t*)veh - g_baseAddress + 0x400000);

		//if (index > 0 && index < 1000) {
		//	Matrix34* pCoords = *((Matrix34**)((uint8_t*)veh + 0x20));
		//	printf("%f\t%f\t%f\n", pCoords->d.x, pCoords->d.y, pCoords->d.z);
		//}

#endif // _DEBUG

	}
	
	return nullptr;

}

void CVehExtParams::init() {
	tVeh::ms_constructor = setFnAddrInCallOpcode(g_hookAddr_CVehicle__CVehicle, getThisCallAddr(&tVeh::CVehicle__CVehicle));
	PRINT_DUBUG("[CVehExtParams::init(...)] %u hooks initialized\n", 1);

	PRINT_DUBUG("struct %s {\n", "CVehExtParams::Params");

	// ToDo: add align
	if (CConfig::ms_bLicensePlates) {
		Params::ms_charsOffset = Params::ms_size;
		Params::ms_size += 12;
		PRINT_DUBUG("\t%s; // +%X\n", "uint8_t m_chars[12]", Params::ms_charsOffset);
	}
	Params::ms_structOffset = Params::ms_size;
	Params::ms_size += sizeof size_t;
	PRINT_DUBUG("\t%s; // +%X\n", "CVehStruct2* m_pStruct", Params::ms_structOffset);

	if (CConfig::ms_bDashboard) {
		Params::ms_speedArrowOffset = Params::ms_size;
		Params::ms_size += sizeof(float);
		PRINT_DUBUG("\t%s; // +%X\n", "float m_fDashboardSpeed", Params::ms_speedArrowOffset);

		Params::ms_rpmArrowOffset = Params::ms_size;
		Params::ms_size += sizeof(float);
		PRINT_DUBUG("\t%s; // +%X\n", "float m_fDashboardRpm", Params::ms_rpmArrowOffset);

		Params::ms_tempArrowOffset = Params::ms_size;
		Params::ms_size += sizeof(float);
		PRINT_DUBUG("\t%s; // +%X\n", "float m_fDashboardTemp", Params::ms_tempArrowOffset);
	}

	if (CConfig::ms_bPatchEngine) {
		if (CConfig::ms_bEngineComponents) {
			Params::ms_engineRotOffset = Params::ms_size;
			Params::ms_size += sizeof(float);
			PRINT_DUBUG("\t%s; // +%X\n", "float m_fEngineRot", Params::ms_engineRotOffset);

			Params::ms_superchargerThrottleRotOffset = Params::ms_size;
			Params::ms_size += sizeof(float);
			PRINT_DUBUG("\t%s; // +%X\n", "float m_fSuperchargerThrottleRot", Params::ms_superchargerThrottleRotOffset);
		}

	}

	if (CConfig::ms_bTankComponents) {

		Params::ms_tankTrackParamsOffset = Params::ms_size;
		Params::ms_size += sizeof(CTankTrackParams);
		PRINT_DUBUG("\t%s; // +%X\n", "CTankTrackParams m_tankTrackParams", Params::ms_tankTrackParamsOffset);

	}

	if (true) {

		Params::ms_useTankSkidmarkOffset = Params::ms_size;
		Params::ms_size += sizeof(bool);
		Params::ms_size = (Params::ms_size + 0xF) & ~0xF;
		PRINT_DUBUG("\t%s; // +%X\n", "bool ms_useTankSkidmarkOffset", Params::ms_useTankSkidmarkOffset);
	}

	if (CConfig::ms_bMoreWheels) {
		Params::ms_size = (Params::ms_size + 0xf) & ~0xf;

		Params::ms_moreWheelsOffset = Params::ms_size;
		Params::ms_size += sizeof(CWheel) * 10;
		PRINT_DUBUG("\t%s; // +%X\n", "CWheel m_moreWheels[10]", Params::ms_moreWheelsOffset);

	}

	if (true) {
		Params::ms_vehWeapOffset = Params::ms_size;
		Params::ms_size += sizeof(CVehicleWeapon);
		PRINT_DUBUG("\t%s; // +%X\n", "CVehicleWeapon m_weapons", Params::ms_vehWeapOffset);
		
	}
	

	PRINT_DUBUG("};\n");


	PRINT_DUBUG("real CVehExtParams::Params::ms_size = %i\n", Params::ms_size);
	Params::ms_size = max((Params::ms_size + 0xF) & ~0xF, 1);
	PRINT_DUBUG("aligned CVehExtParams::Params::ms_size = %i\n", Params::ms_size);

}

// new variables
size_t CCustomShaderEffectVehicleFX::ms_updateOrigcall = 0;
size_t CCustomShaderEffectVehicleFX::ms_initOrigcall = 0;
size_t CCustomShaderEffectVehicleFX::ms_setVarsOrigcall = 0;


CCustomShaderVehFXAddons g_customVehFxAddons[0x10];
uint8_t g_numCustomVehFxAddons = 0;

char CCustomShaderEffectVehicleFX::update(CVehicle* pVeh) {
	auto ret = ((char(__thiscall*)(CCustomShaderEffectVehicleFX*, CVehicle*))(ms_updateOrigcall))(this, pVeh);

	auto pVars = getAddonVars();


	if (auto pExtParams = g_extVehParams.getFromVehicle(pVeh))
		for (size_t i = 0; i < g_numCustomVehFxAddons; i++)
			g_customVehFxAddons[i].m_pfnUpdate(this, pVeh, pExtParams, pVars);

	return ret;
}


void CCustomShaderEffectVehicleFX::setVars() {
	((void(__thiscall*)(CCustomShaderEffectVehicleFX*))(ms_setVarsOrigcall))(this);

	auto pVars = getAddonVars();

	//if(pVars->m_nCringeLevelIndex)
	//	((void(__thiscall*)(grmShaderGroup*, int groupIndex, float val))(g_rage__grmShaderGroup__SetVarFloat))
	//	(m_pShaderGroup, pVars->m_nCringeLevelIndex, pVars->m_fCringeLevel);

	for (size_t i = 0; i < g_numCustomVehFxAddons; i++)
		g_customVehFxAddons[i].m_pfnSetVars(this, pVars);

}


char CCustomShaderEffectVehicleFX::init(void* pDrawableRef) {
	auto ret = ((char(__thiscall*)(CCustomShaderEffectVehicleFX*, void*))(ms_initOrigcall))(this, pDrawableRef);

	auto pVars = getAddonVars();

	//pVars->m_nCringeLevelIndex = ((int(__thiscall*)(grmShaderGroup*, const char*, bool bRequired))(g_rage__grmShaderGroup__addShaderGroupVar))(m_pShaderGroup, "dirtLevel", true);
	for (size_t i = 0; i < g_numCustomVehFxAddons; i++)
		g_customVehFxAddons[i].m_pfnInit(this, pVars);


	return ret;
}

void CCustomShaderEffectVehicleFX::patchSize() {
	uint32_t oldSize = *(uint32_t*)g_CCustomShaderEffectVehicleFX_struct_size;
	uint32_t newSize = oldSize + sizeof CAddonVars;
	writeDWORD(g_CCustomShaderEffectVehicleFX_struct_size, newSize);

	PRINT_DUBUG("[CCustomShaderEffectVehicleFX::patchSize(...)] old size = %u, new size = %u\n", oldSize, newSize);

}

void CCustomShaderEffectVehicleFX::initHooks() {
	if (!CConfig::ms_bPatchCustomVehFX)
		return;


	if (CConfig::ms_bLicensePlates) {
		g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnInit = initLicensePlates;
		g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnSetVars = setVarsLicensePlates;
		g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnUpdate = updateLicensePlates;
		g_numCustomVehFxAddons++;
	}

	if (!g_numCustomVehFxAddons)
		return;

	ms_updateOrigcall = writeDWORD(g_vmtAddr__CCustomShaderEffectVehicleFX__update, getThisCallAddr(&update));
	ms_initOrigcall = writeDWORD(g_vmtAddr__CCustomShaderEffectVehicleFX__init, getThisCallAddr(&init));
	ms_setVarsOrigcall = writeDWORD(g_vmtAddr__CCustomShaderEffectVehicleFX__setShaderVars, getThisCallAddr(&setVars));
	patchSize();


	PRINT_DUBUG("[CCustomShaderEffectVehicleFX::initHooks(...)] %u hooks initialized in vmt\n", 3);

}


// добавляем новые нативные функции

size_t rage__scrThread__registerCommand_origCall;

__forceinline char __cdecl rage__scrThread__registerCommand(uint32_t hash, size_t pfn) {
	return ((char(__cdecl*)(unsigned int hash, size_t pfn))(rage__scrThread__registerCommand_origCall))(hash, pfn);
}

struct native_obj {
	int32_t _f0;
	int32_t _f4;
	uint32_t* _f8;
	int32_t _fC;
};


// SET_CAR_LICENSE_PLATE_TEXT_0123 = 0x09ded943
// SET_CAR_LICENSE_PLATE_TEXT_4567 = 0x09de9587
// SET_CAR_LICENSE_PLATE_TEXT_891011 = 0x0e515a91


int __cdecl vehicleScript::setCarLicensePlateText0123(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pChars = params->getPlateChars();

		//PRINT_DUBUG("%i before:", vehHandle);
		//for (size_t i = 0; i < 8; i++)
		//	PRINT_DUBUG(" %i", pChars[i]);
		//PRINT_DUBUG("\n");

		pChars[0] = char0;
		pChars[1] = char1;
		pChars[2] = char2;
		pChars[3] = char3;

		return TRUE;
	}
	else 
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_0123: veh handle %i out of bounds\n", vehHandle);
	return FALSE;
}

int __cdecl vehicleScript::setCarLicensePlateText4567(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pChars = params->getPlateChars();
		pChars[4] = char0;
		pChars[5] = char1;
		pChars[6] = char2;
		pChars[7] = char3;

		return TRUE;
	}
	else 
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_4567: veh index %i out of bounds\n", vehHandle);

	return FALSE;
}

int __cdecl vehicleScript::setCarLicensePlateText891011(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pChars = params->getPlateChars();
		pChars[8] = char0;
		pChars[9] = char1;
		pChars[10] = char2;
		pChars[11] = char3;
		
		return TRUE;
	}
	else 
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_891011: veh handle %i out of bounds\n", vehHandle);

	return FALSE;
}

int __cdecl vehicleScript::n_nullCall(native_obj* _a) {
	return TRUE;
}
int __cdecl vehicleScript::n_setCarLicensePlateText0123(native_obj* _a) {
	return setCarLicensePlateText0123(_a->_f8[0], _a->_f8[1], _a->_f8[2], _a->_f8[3], _a->_f8[4]);
}
int __cdecl vehicleScript::n_setCarLicensePlateText4567(native_obj* _a) {
	return setCarLicensePlateText4567(_a->_f8[0], _a->_f8[1], _a->_f8[2], _a->_f8[3], _a->_f8[4]);
}
int __cdecl vehicleScript::n_setCarLicensePlateText891011(native_obj* _a) {
	return setCarLicensePlateText891011(_a->_f8[0], _a->_f8[1], _a->_f8[2], _a->_f8[3], _a->_f8[4]);
}



void __cdecl vehicleScript::registerNewCommands() {
	if (CConfig::ms_bLicensePlates) {
		rage__scrThread__registerCommand(0x09ded943, (size_t)n_setCarLicensePlateText0123);
		PRINT_DUBUG("[vehicleScript::registerNewCommands] added command %s as %#010x\n", "vehicleScript::setCarLicensePlateText0123", 0x09DED943);

		rage__scrThread__registerCommand(0x09de9587, (size_t)n_setCarLicensePlateText4567);
		PRINT_DUBUG("[vehicleScript::registerNewCommands] added command %s as %#010x\n", "vehicleScript::setCarLicensePlateText4567", 0x09de9587);

		rage__scrThread__registerCommand(0x0e515a91, (size_t)n_setCarLicensePlateText891011);
		PRINT_DUBUG("[vehicleScript::registerNewCommands] added command %s as %#010x\n", "vehicleScript::setCarLicensePlateText891011", 0x0e515a91);

	}
	else {
		rage__scrThread__registerCommand(0x09ded943, (size_t)n_nullCall);
		PRINT_DUBUG("[vehicleScript::registerNewCommands] added command %s as %#010x\n", "vehicleScript::nullCall", 0x09DED943);
		
		rage__scrThread__registerCommand(0x09de9587, (size_t)n_nullCall);
		PRINT_DUBUG("[vehicleScript::registerNewCommands] added command %s as %#010x\n", "vehicleScript::nullCall", 0x09de9587);

		rage__scrThread__registerCommand(0x0e515a91, (size_t)n_nullCall);
		PRINT_DUBUG("[vehicleScript::registerNewCommands] added command %s as %#010x\n", "vehicleScript::nullCall", 0x0e515a91);
	}

}

char __cdecl vehicleScript::setupScriptCommands_hook(uint32_t hash, size_t pfn) {
	auto retVal = rage__scrThread__registerCommand(hash, pfn);

	registerNewCommands();

	return retVal;
}


void vehicleScript::init() {
	rage__scrThread__registerCommand_origCall = setFnAddrInCallOpcode(g_hookAddr_registerNative, (size_t)setupScriptCommands_hook);
	PRINT_DUBUG("[vehicleScript::init(...)] %u hooks initialized\n", 1);

}


void rage__grmShaderGroup__setVarVector4(grmShaderGroup* pShaderGroup, int32_t groupIndex, float* pVec) {
	((void(__thiscall*)(grmShaderGroup * pShaderGroup, int groupIndex, float* pVec))(g_rage__grmShaderGroup__SetVarVector4))(pShaderGroup, groupIndex, pVec);
}

uint32_t rage__grmShaderGroup__addShaderGroupVar(grmShaderGroup* pShaderGroup, const char* pszName, bool bRequired) {
	return ((uint32_t(__thiscall*)(grmShaderGroup*, const char*, bool bRequired))(g_rage__grmShaderGroup__addShaderGroupVar))(pShaderGroup, pszName, bRequired);
}
