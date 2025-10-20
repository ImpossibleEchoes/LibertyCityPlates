#include "licensePlates.h"
#include "addrs.h"
#include "trace.h"
#include "atPool.h"
#include "helpers.h"
#include "utils.h"
#include "CVehicle.h"

#include <map>

std::map<uint32_t, uint32_t> g_vehPlates;

void parseCarPlatesIde(char* line) {
	char tokChars[4];
	strcpy(tokChars, " \t");

	auto tok = strtok(line, tokChars);

	PRINT_DUBUG("reading plates for %s\n", tok);

	auto namehash = jenkins_one_at_a_time_hash(tok);

	tok = strtok(nullptr, tokChars);
	uint32_t index = atoi(tok);

	if (g_vehPlates.count(namehash))
		g_vehPlates[namehash] = index;
	else
		g_vehPlates.insert(std::make_pair(namehash, index));

}


void __cdecl updateLicensePlates(CCustomShaderEffectVehicleFX*, CVehicle*, CVehExtParams::Params* pExtParams,
	CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	auto pChars = pExtParams->getPlateChars();
	if (pVars->m_nLetters0123Index > 0) {
		pVars->m_aLetters0123[0] = pChars[0];
		pVars->m_aLetters0123[1] = pChars[1];
		pVars->m_aLetters0123[2] = pChars[2];
		pVars->m_aLetters0123[3] = pChars[3];
	}

	if (pVars->m_nLetters4567Index > 0) {
		pVars->m_aLetters4567[0] = pChars[4];
		pVars->m_aLetters4567[1] = pChars[5];
		pVars->m_aLetters4567[2] = pChars[6];
		pVars->m_aLetters4567[3] = pChars[7];
	}

	if (pVars->m_nLetters891011Index > 0) {
		pVars->m_aLetters891011[0] = pChars[8];
		pVars->m_aLetters891011[1] = pChars[9];
		pVars->m_aLetters891011[2] = pChars[10];
		pVars->m_aLetters891011[3] = pChars[11];
	}

}
void __cdecl setVarsLicensePlates(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	if (pVars->m_nLetters0123Index)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLetters0123Index, pVars->m_aLetters0123);
	if (pVars->m_nLetters4567Index)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLetters4567Index, pVars->m_aLetters4567);
	if (pVars->m_nLetters891011Index)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLetters891011Index, pVars->m_aLetters891011);
}

void __cdecl initLicensePlates(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	pVars->m_nLetters0123Index = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "letters0123", false);
	pVars->m_nLetters4567Index = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "letters4567", false);
	pVars->m_nLetters891011Index = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "letters891011", false);
}

void __cdecl setStandartLicensePlate(CVehicle* pVeh, CVehExtParams::Params* pParams) {
	uint32_t type1 = g_pfnGetVehMainType(pVeh);
	uint32_t type2 = g_pfnGetVehSpecialType(pVeh);

	auto pChars = pParams->getPlateChars();

	auto mdl = g_modelPointers[pVeh->getModelIndex()];
	if (g_vehPlates.count(mdl->m_hash)) {
		auto index = g_vehPlates[mdl->m_hash];

		switch (index) {
		case 1: { // police/ambulance
			uint32_t _r0 = ((int32_t(__cdecl*)())g_rand)();
			_r0 <<= 15; // RAND_MAX
			_r0 |= ((int32_t(__cdecl*)())g_rand)();
			pChars[0] = pChars[1] = pChars[6] = pChars[7] = 16 * 2 + 5;
			for (size_t i = 2; i < 6; i++)
				pChars[i] = (_r0 >> ((i - 2) * 6) & 0x3F) % 10;
			break;
		}
		case 2: { // fbi
			uint32_t _r0 = ((int32_t(__cdecl*)())g_rand)();
			_r0 <<= 15; // RAND_MAX
			_r0 |= ((int32_t(__cdecl*)())g_rand)();
			pChars[0] = pChars[7] = 16 * 2 + 5;
			for (size_t i = 1; i < 7; i++) {
				bool bIsChar = !(_r0 >> ((i - 1) * 4) % 3);
				if (bIsChar) pChars[i] = ((_r0 >> ((i - 1) * 4) & 0x1F) % 26) + 10;
				else pChars[i] = (_r0 >> ((i - 1) * 4) & 0x1F) % 10;
			}
			break;
		}
		}
		return;
	}


	switch (type1) {
	case 0:
	case 1:

		switch (type2) {
		case 0: // car
		case 1: // bike
		{
			// Get a random 15 bits number, 5 bits per symbol
			uint32_t _r0 = ((int32_t(__cdecl*)())g_rand)();

			for (size_t i = 0; i < 3; i++)
				pChars[i] = ((_r0 >> 5 * i) % 26) + 10;

			pChars[3] = 0x24;

			// Get a random 30 bits number
			_r0 = ((int32_t(__cdecl*)())g_rand)();
			_r0 <<= 15; // RAND_MAX
			_r0 |= ((int32_t(__cdecl*)())g_rand)();

			for (size_t i = 0; i < 4; i++)
				pChars[4 + i] = (_r0 >> 4 * i) % 10;

			if (type1 == 1)
				pChars[7] = 16 * 2 + 5;

		}
		break;
		case 4: // heli
		case 5: // plane
			//pParams->m_aChars[0] = 7 + 0x10; // N
			//pParams->m_aChars[1] = 5 + 0x20; // -
			//pParams->m_aChars[2] = 5 + 0x10; // L
			//pParams->m_aChars[3] = 0xc; // C
			break;
		}

		break;

	case 2: // boat
		break;
	}

}