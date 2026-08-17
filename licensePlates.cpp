#include "licensePlates.h"
#include "addrs.h"
#include "trace.h"
#include "hookFns.h"
#include "atPool.h"
#include "helpers.h"
#include "utils.h"
#include "CVehicle.h"
#include "config.h"
#include "rage_base.h"
#include "newComponents.h"
#include "helpers.h"

#include <map>
#include <vector>

struct grcTexture;


const char* CPlateFactory::PlateTxds::ms_pszDictName = "libertycityplates";
const char* CPlateFactory::PlateTxds::ms_DictPath = "platform:/textures/libertycityplates";
uint32_t CPlateFactory::PlateTxds::ms_LibertyCityPlatesTxd = ~0;
size_t CPlateFactory::PlateTxds::ms_load;
size_t CPlateFactory::PlateTxds::ms_release;

std::map<uint32_t, std::vector<uint8_t>> CPlateFactory::ms_vehPlates;
CPlateFactory::PlateTxdsTable CPlateFactory::ms_plateTexturesHashes;
CPlateFactory::PlateInfo CPlateFactory::ms_plates[CPlateFactory::MAX_NUM_PLATES];



void* CPlateFactory::PlateTxds::load() {

	ms_LibertyCityPlatesTxd = ((uint32_t(__cdecl*)(const char*))(g_CTxdStore__getIndexByName))(ms_pszDictName);
	if (ms_LibertyCityPlatesTxd == ~0) {
		ms_LibertyCityPlatesTxd = ((uint32_t(__cdecl*)(const char*))(g_CTxdStore__addEntry))(ms_pszDictName);
		if (((bool(__cdecl*)(uint32_t, const char*))(g_CTxdStore__loadFile))(ms_LibertyCityPlatesTxd, ms_DictPath)) {
			((uint32_t(__cdecl*)(uint32_t))(g_CTxdStore__addRef))(ms_LibertyCityPlatesTxd);
			auto pDict = ((pgDictionary<grcTexture>*(__cdecl*)(uint32_t))(g_CTxdStore__at))(ms_LibertyCityPlatesTxd);
			PRINT_DUBUG("loaded global script txd dict at $%p with id %i\n", pDict, ms_LibertyCityPlatesTxd);

		}
		else
			ms_LibertyCityPlatesTxd = ~0;
	}

	return ((void* (*)())(ms_load))();
}

void* CPlateFactory::PlateTxds::release() {
	if (ms_LibertyCityPlatesTxd != ~0) {
		((uint32_t(__cdecl*)(uint32_t))(g_CTxdStore__release))(ms_LibertyCityPlatesTxd);
		((uint32_t(__cdecl*)(uint32_t))(g_CTxdStore__releaseEntry))(ms_LibertyCityPlatesTxd);
		//ms_LibertyCityPlatesTxd = ~0;
		ms_LibertyCityPlatesTxd = ((uint32_t(__cdecl*)(const char*))(g_CTxdStore__getIndexByName))(ms_pszDictName);

		//

		auto pDict = ((pgDictionary<grcTexture>*(__cdecl*)(uint32_t))(g_CTxdStore__at))(ms_LibertyCityPlatesTxd);
		PRINT_DUBUG("released global script txd dict\n");

#ifdef _DEBUG
		if (ms_LibertyCityPlatesTxd != ~0)
			printDebug("failed to release txd dict\n");
		else
			printDebug("txd dict was released correctly\n");
#endif // _DEBUG
	}
	else
		PRINT_DUBUG("nothing to release\n");

	return ((void* (*)())(ms_release))();
}


void CPlateFactory::PlateTxds::init() {
	ms_load = setFnAddrInCallOpcode(g_hookAddr_loadPlateTxd, (size_t)load);
	ms_release = setFnAddrInCallOpcode(g_hookAddr_releasePlateTxd, (size_t)release);
}


void CPlateFactory::PlateTxdsTable::init() {
	char msg[0x7F];
	for (size_t i = 0; i < MAX_NUM_PLATES_TEXTURES; i++) {
		sprintf(msg, "plate%u_d", i);
		m_aTextures[i].m_Plate_d = jenkins_one_at_a_time_hash(msg);
		sprintf(msg, "plate%u_n", i);
		m_aTextures[i].m_Plate_n = jenkins_one_at_a_time_hash(msg);
		sprintf(msg, "plate%u_s", i);
		m_aTextures[i].m_Plate_s = jenkins_one_at_a_time_hash(msg);

		sprintf(msg, "letters%u_d", i);
		m_aTextures[i].m_Letters_d = jenkins_one_at_a_time_hash(msg);
		sprintf(msg, "letters%u_n", i);
		m_aTextures[i].m_Letters_n = jenkins_one_at_a_time_hash(msg);
	}
}



// read
void CPlateFactory::parseCarPlatesIde(char* line) {
	char tokChars[4];
	strcpy(tokChars, " \t");

	auto tok = strtok(line, tokChars);

	PRINT_DUBUG("reading plates for %s:", tok);


	auto namehash = getOrParseHash(tok);


	std::vector<uint8_t>* pUsedPlates;
	if (ms_vehPlates.count(namehash)) {
		pUsedPlates = &ms_vehPlates[namehash];
	}
	else {
		ms_vehPlates.insert(std::make_pair(namehash, std::vector<uint8_t>()));
		pUsedPlates = &ms_vehPlates[namehash];
	}

	tok = strtok(nullptr, tokChars);
	while (tok != nullptr) {
		pUsedPlates->push_back(atoi(tok));

		PRINT_DUBUG(" %i", atoi(tok));

		tok = strtok(nullptr, tokChars);
	}
	PRINT_DUBUG("\n");

	if (pUsedPlates->size() < 1)
		MessageBoxA(nullptr, "empty entry in used_plates not allowed", nullptr, 0x10);
}

void CPlateFactory::parseLicensePlatesIde(char* line) {
	char tokChars[4];
	strcpy(tokChars, " \t");

	auto tok = strtok(line, tokChars);

	PRINT_DUBUG("reading plates for %s\n", tok);

	uint32_t index = atoi(tok);

	tok = strtok(nullptr, tokChars);
	if (!strcmp(tok, "null"))
		ms_plates[index].m_ParentId = ~0;
	else {
		auto parentId = atoi(tok);
		if (parentId < 0 || parentId >= MAX_NUM_PLATES)
			ms_plates[index].m_ParentId = ~0;
		else
			ms_plates[index].m_ParentId = parentId;
	}

	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_Chance = atoi(tok);


	tok = strtok(nullptr, tokChars);
	uint32_t tmp = atoi(tok);
	ms_plates[index].m_PlateTxdId = tmp < MAX_NUM_PLATES_TEXTURES ? tmp : 0;


	tok = strtok(nullptr, tokChars);
	tmp = atoi(tok);
	ms_plates[index].m_LettersTxdId = tmp < MAX_NUM_PLATES_TEXTURES ? tmp : 0;

	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_vecLettersScale.x = atof(tok);
	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_vecLettersScale.y = atof(tok);

	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_vecLettersOffset.x = atof(tok);
	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_vecLettersOffset.y = atof(tok);

	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_vecLettersTint.x = atof(tok);

	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_vecLettersTint.y = atof(tok);

	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_vecLettersTint.z = atof(tok);

	tok = strtok(nullptr, tokChars);
	ms_plates[index].m_vecLettersTint.w = atof(tok);

	tok = strtok(nullptr, tokChars);
	//tmp = atoi(tok);
	//ms_plates[index].m_NumLetters = tmp;

	ms_plates[index].m_NumLetters = 0;

	for (size_t i = 0; i < strlen(tok); i++) {
		switch (tok[i]) {
		case '&':
			ms_plates[index].m_aFormat[i] = PLATE_CHAR_RANDOM;
			break;
		case '*':
			ms_plates[index].m_aFormat[i] = 0x24;
			break;
		case '^':
			ms_plates[index].m_aFormat[i] = PLATE_NUM_RANDOM;
			break;
		case '%':
			ms_plates[index].m_aFormat[i] = PLATE_FULL_RANDOM;
			break;
		case '#':
			ms_plates[index].m_aFormat[i] = 0x25;
			break;
		case '(':
			ms_plates[index].m_aFormat[i] = PLATE_LOWERCASE_AB_RANDOM;
			break;
		default:
			// to uppercase
			if (tok[i] >= 'a' && tok[i] <= 'z')
				tok[i] -= 0x20;

			if (tok[i] >= 'A' && tok[i] <= 'Z')
				ms_plates[index].m_aFormat[i] = tok[i] - 'A' + 10;
			else if (tok[i] >= '0' && tok[i] <= '9')
				ms_plates[index].m_aFormat[i] = tok[i] - '0';
			else
				ms_plates[index].m_aFormat[i] = 0x25;

			break;
		}

		ms_plates[index].m_NumLetters++;

	}


	tok = strtok(nullptr, tokChars);
	int numZones = atoi(tok);
	if (numZones == 0)
		ms_plates[index].m_rects.clear();
	else if (ms_plates[index].m_rects.size() != numZones)
		ms_plates[index].m_rects.resize(numZones);

	for (size_t i = 0; i < ms_plates[index].m_rects.size(); i++) {
		for (size_t j = 0; j < 4; j++) {
			tok = strtok(nullptr, tokChars);
			(&ms_plates[index].m_rects[i].minX)[j] = atof(tok);
		}
	}

}


// export 
void CPlateFactory::exportCarPlatesIde(FILE* f) {
	for (auto& entry : ms_vehPlates) {
		fprintf(f, "hash:%i", entry.first);

		for (auto& entry2 : entry.second)
			fprintf(f, " %i", (int)entry2);
		fprintf(f, "\n");
	}
}

void CPlateFactory::exportLicensePlatesIde(FILE* f) {
	for (size_t i = 0; i < MAX_NUM_PLATES; i++) {
		fprintf(f, "%i ", i);
		if (ms_plates[i].m_ParentId >= MAX_NUM_PLATES)
			fprintf(f, "null");
		else
			fprintf(f, "%i", ms_plates[i].m_ParentId);

		fprintf(f, " %i %i %i %f %f %f %f %f %f %f %f ",
			ms_plates[i].m_Chance,
			ms_plates[i].m_PlateTxdId, ms_plates[i].m_LettersTxdId,
			ms_plates[i].m_vecLettersScale.x, ms_plates[i].m_vecLettersScale.y,
			ms_plates[i].m_vecLettersOffset.x, ms_plates[i].m_vecLettersOffset.y,
			ms_plates[i].m_vecLettersTint.x, ms_plates[i].m_vecLettersTint.y, ms_plates[i].m_vecLettersTint.z, ms_plates[i].m_vecLettersTint.w);

		if (ms_plates[i].m_NumLetters > 0) {
			size_t j = 0;
			do {
				switch (ms_plates[i].m_aFormat[j]) {
				case PLATE_CHAR_RANDOM:
					fprintf(f, "&");
					break;
				case 0x24:
					fprintf(f, "*");
					break;
				case PLATE_NUM_RANDOM:
					fprintf(f, "^");
					break;
				case PLATE_FULL_RANDOM:
					fprintf(f, "%%");
					break;
				case 0x25:
					fprintf(f, "#");
					break;
				case PLATE_LOWERCASE_AB_RANDOM:
					fprintf(f, "(");
					break;
				default:
					if (ms_plates[i].m_aFormat[j] < 10)
						fprintf(f, "%i", ms_plates[i].m_aFormat[j]);
					else
						fprintf(f, "%c", ms_plates[i].m_aFormat[j] + 'A' - 10);


					break;
				}

				j++;
			} while (j < ms_plates[i].m_NumLetters);
		}
		else
			fprintf(f, "null");

		fprintf(f, " %i", ms_plates[i].m_rects.size());
		for (size_t j = 0; j < ms_plates[i].m_rects.size(); j++)
			fprintf(f, " %f %f %f %f", ms_plates[i].m_rects[j].minX, ms_plates[i].m_rects[j].maxX, ms_plates[i].m_rects[j].minY, ms_plates[i].m_rects[j].maxY);

		fprintf(f, "\n");
	}

}

// shaderFX
void __cdecl CPlateFactory::updateLicensePlates(CCustomShaderEffectVehicleFX*, CVehicle* pVeh, CVehExtParams::Params* pExtParams,
	CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	auto pPlateInfo = pExtParams->getPlateInfo();
	auto pChars = pPlateInfo->m_aLicensePlateText;
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

	if (pVars->m_nLicensePlateValuesIndex > 0) {
		pVars->m_aLicensePlateValues[0] = 12 - pPlateInfo->m_NumChars;
		pVars->m_aLicensePlateValues[1] = pPlateInfo->m_NumChars;
	}

	if (pVars->m_nLettersTintIndex > 0) {
		pVars->m_aLettersTint[0] = pPlateInfo->m_vecPlateTextTint.x;
		pVars->m_aLettersTint[1] = pPlateInfo->m_vecPlateTextTint.y;
		pVars->m_aLettersTint[2] = pPlateInfo->m_vecPlateTextTint.z;
		pVars->m_aLettersTint[3] = pPlateInfo->m_vecPlateTextTint.w;
	}

	if (pVars->m_nLettersScaleIndex > 0) {
		pVars->m_aLettersScale[0] = pPlateInfo->m_vecLettersScaleAndOffset.x;
		pVars->m_aLettersScale[1] = pPlateInfo->m_vecLettersScaleAndOffset.y;
	}

	if (pVars->m_nLettersOffsetIndex > 0) {
		pVars->m_aLettersOffset[0] = pPlateInfo->m_vecLettersScaleAndOffset.z;
		pVars->m_aLettersOffset[1] = pPlateInfo->m_vecLettersScaleAndOffset.w;
	}

	if (pVars->m_nLettersTxdDifIndex > 0 || pVars->m_nLettersTxdNrmIndex > 0 || pVars->m_nPlateTxdDifIndex > 0 || pVars->m_nPlateTxdNrmIndex > 0 || pVars->m_nPlateTxdSpecIndex > 0) {
		if (pVars->m_nLettersTxdId != pPlateInfo->m_LettersTexIndex || pVars->m_nPlateTxdId != pPlateInfo->m_PlateTexIndex) {
			pgDictionary<grcTexture>* pDict = nullptr;
			pgDictionary<grcTexture>* pGlobalDict = nullptr;

			auto txdId = g_modelPointers[pVeh->getModelIndex()]->m_TxdId;
			if (txdId != ~0)
				pDict = ((pgDictionary<grcTexture>*(__cdecl*)(uint32_t))(g_CTxdStore__at))(txdId);
			if (PlateTxds::ms_LibertyCityPlatesTxd != ~0)
				pGlobalDict = ((pgDictionary<grcTexture>*(__cdecl*)(uint32_t))(g_CTxdStore__at))(PlateTxds::ms_LibertyCityPlatesTxd);

			if (pVars->m_nLettersTxdDifIndex > 0) {
				if (pDict)
					pVars->m_pLetters_d = pDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_LettersTexIndex].m_Letters_d);
				if (!pVars->m_pLetters_d && pGlobalDict)
					pVars->m_pLetters_d = pGlobalDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_LettersTexIndex].m_Letters_d);
			}
			if (pVars->m_nLettersTxdNrmIndex > 0) {
				if (pDict)
					pVars->m_pLetters_n = pDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_LettersTexIndex].m_Letters_n);
				if (!pVars->m_pLetters_n && pGlobalDict)
					pVars->m_pLetters_n = pGlobalDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_LettersTexIndex].m_Letters_n);
			}


			if (pVars->m_nPlateTxdDifIndex > 0) {
				if (pDict)
					pVars->m_pPlate_d = pDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_PlateTexIndex].m_Plate_d);
				if (!pVars->m_pPlate_d && pGlobalDict)
					pVars->m_pPlate_d = pGlobalDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_PlateTexIndex].m_Plate_d);
			}

			if (pVars->m_nPlateTxdNrmIndex > 0) {
				if (pDict)
					pVars->m_pPlate_n = pDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_PlateTexIndex].m_Plate_n);
				if (!pVars->m_pPlate_n && pGlobalDict)
					pVars->m_pPlate_n = pGlobalDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_PlateTexIndex].m_Plate_n);
			}

			if (pVars->m_nPlateTxdSpecIndex > 0) {
				if (pDict)
					pVars->m_pPlate_s = pDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_PlateTexIndex].m_Plate_s);
				if (!pVars->m_pPlate_s && pGlobalDict)
					pVars->m_pPlate_s = pGlobalDict->lookup(ms_plateTexturesHashes.m_aTextures[pPlateInfo->m_PlateTexIndex].m_Plate_s);
			}

			pVars->m_nLettersTxdId = pPlateInfo->m_LettersTexIndex;
			pVars->m_nPlateTxdId = pPlateInfo->m_PlateTexIndex;

		}
	}
}

void __cdecl CPlateFactory::setVarsLicensePlates(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	if (pVars->m_nLetters0123Index)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLetters0123Index, pVars->m_aLetters0123);
	if (pVars->m_nLetters4567Index)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLetters4567Index, pVars->m_aLetters4567);
	if (pVars->m_nLetters891011Index)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLetters891011Index, pVars->m_aLetters891011);
	if (pVars->m_nLicensePlateValuesIndex)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLicensePlateValuesIndex, pVars->m_aLicensePlateValues);
	if (pVars->m_nLettersTintIndex)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLettersTintIndex, pVars->m_aLettersTint);
	if (pVars->m_nLettersScaleIndex)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLettersScaleIndex, pVars->m_aLettersScale);
	if (pVars->m_nLettersOffsetIndex)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nLettersOffsetIndex, pVars->m_aLettersOffset);

	if (pVars->m_nPlateTxdDifIndex && pVars->m_pPlate_d)
		rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nPlateTxdDifIndex, pVars->m_pPlate_d);
	if (pVars->m_nPlateTxdNrmIndex && pVars->m_pPlate_n)
		rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nPlateTxdNrmIndex, pVars->m_pPlate_n);
	if (pVars->m_nPlateTxdSpecIndex && pVars->m_pPlate_s)
		rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nPlateTxdSpecIndex, pVars->m_pPlate_s);

	if (pVars->m_nLettersTxdDifIndex && pVars->m_pLetters_d)
		rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nLettersTxdDifIndex, pVars->m_pLetters_d);
	if (pVars->m_nLettersTxdNrmIndex && pVars->m_pLetters_n)
		rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nLettersTxdNrmIndex, pVars->m_pLetters_n);

	//if (pVars->m_nPlateTxdDifIndex)
	//	rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nPlateTxdDifIndex, g_plateTextures[pVars->m_nPlateTxdId].m_pDiffuse);
	//if (pVars->m_nPlateTxdNrmIndex)
	//	rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nPlateTxdNrmIndex, g_plateTextures[pVars->m_nPlateTxdId].m_pNormal);
	//if (pVars->m_nPlateTxdSpecIndex)
	//	rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nPlateTxdSpecIndex, g_plateTextures[pVars->m_nPlateTxdId].m_pSpecular);

	//if (pVars->m_nLettersTxdDifIndex)
	//	rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nLettersTxdDifIndex, g_lettersTextures[pVars->m_nLettersTxdId].m_pDiffuse);
	//if (pVars->m_nLettersTxdNrmIndex)
	//	rage__grmShaderGroup__setVarTexture(pFx->m_pShaderGroup, pVars->m_nLettersTxdNrmIndex, g_lettersTextures[pVars->m_nLettersTxdId].m_pNormal);
}

void __cdecl CPlateFactory::initLicensePlates(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	pVars->m_nLetters0123Index = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "letters0123", false);
	pVars->m_nLetters4567Index = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "letters4567", false);
	pVars->m_nLetters891011Index = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "letters891011", false);
	pVars->m_nLicensePlateValuesIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "licenseplatevalues", false);
	pVars->m_nLettersTintIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "letterstint", false);
	pVars->m_nLettersScaleIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "lettersscale", false);
	pVars->m_nLettersOffsetIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "lettersoffset", false);

	pVars->m_nLettersTxdDifIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "letterssampler", false);
	pVars->m_nLettersTxdNrmIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "lettersbumpsampler", false);
	pVars->m_nPlateTxdDifIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "platetexturesampler", false);
	pVars->m_nPlateTxdNrmIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "platebumpsampler", false);
	pVars->m_nPlateTxdSpecIndex = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "platespecsampler", false);

	pVars->m_pPlate_d = nullptr;
	pVars->m_pPlate_n = nullptr;
	pVars->m_pPlate_s = nullptr;
	pVars->m_pLetters_d = nullptr;
	pVars->m_pLetters_n = nullptr;
	pVars->m_nLettersTxdId = ~0;
	pVars->m_nPlateTxdId = ~0;
}

void CPlateFactory::setPlateParams(CVehicle* pVeh, CVehExtParams::Params* pParams, int index) {
	auto& plate = ms_plates[index];

	if (plate.m_ParentId != _UI8_MAX) {
		if (plate.m_Chance != _UI8_MAX) {
			auto random = ((int32_t(__cdecl*)())g_rand)() % 100;
			if (plate.m_Chance < random) {
				setPlateParams(pVeh, pParams, plate.m_ParentId);
				return;
			}
		}
		auto pMtx = pVeh->getTransform();
		for (const auto& rect : plate.m_rects) {
			if (!(pMtx->d.x > rect.minX && pMtx->d.x < rect.maxX && pMtx->d.y > rect.minY && pMtx->d.y < rect.maxY)) {
				setPlateParams(pVeh, pParams, plate.m_ParentId);
				return;
			}

		}
	}

	auto plateInfo = pParams->getPlateInfo();
	plateInfo->m_NumChars = ms_plates[index].m_NumLetters;
	plateInfo->m_PlateTexIndex = plate.m_PlateTxdId;
	plateInfo->m_LettersTexIndex = plate.m_LettersTxdId;

	for (size_t i = 0; i < plate.m_NumLetters; i++) {
		switch (plate.m_aFormat[i]) {

		case PLATE_FULL_RANDOM:
			plateInfo->m_aLicensePlateText[i] = getRandom(0, 35); // 0x25 = 37
			break;
		case PLATE_NUM_RANDOM:
			plateInfo->m_aLicensePlateText[i] = getRandom(0, 9);
			break;
		case PLATE_CHAR_RANDOM:
			plateInfo->m_aLicensePlateText[i] = getRandom(10, 35);
			break;
		case PLATE_LOWERCASE_AB_RANDOM:
			plateInfo->m_aLicensePlateText[i] = getRandom(38, 39); // 0x26 = 38
			break;
		default:
			plateInfo->m_aLicensePlateText[i] = plate.m_aFormat[i];
			break;
		}

	}

	plateInfo->m_vecLettersScaleAndOffset.x = ms_plates[index].m_vecLettersScale.x;
	plateInfo->m_vecLettersScaleAndOffset.y = ms_plates[index].m_vecLettersScale.y;
	plateInfo->m_vecLettersScaleAndOffset.z = ms_plates[index].m_vecLettersOffset.x;
	plateInfo->m_vecLettersScaleAndOffset.w = ms_plates[index].m_vecLettersOffset.y;

	plateInfo->m_vecPlateTextTint.x = ms_plates[index].m_vecLettersTint.x;
	plateInfo->m_vecPlateTextTint.y = ms_plates[index].m_vecLettersTint.y;
	plateInfo->m_vecPlateTextTint.z = ms_plates[index].m_vecLettersTint.z;
	plateInfo->m_vecPlateTextTint.w = ms_plates[index].m_vecLettersTint.w;

}

void __cdecl CPlateFactory::setStandartLicensePlate(CVehicle* pVeh, CVehExtParams::Params* pParams) {
	uint32_t type1 = g_pfnGetVehMainType(pVeh);
	uint32_t type2 = g_pfnGetVehSpecialType(pVeh);

	auto pChars = pParams->getPlateInfo()->m_aLicensePlateText;
	//auto pChars = pParams->getPlateChars();

	auto mdl = g_modelPointers[pVeh->getModelIndex()];
	if (ms_vehPlates.count(mdl->m_hash)) {
		auto index = ms_vehPlates[mdl->m_hash];


		setPlateParams(pVeh, pParams, getRandomElementFromVector(index));

		return;
	}


	switch (type1) {
	case 0:
	case 1:

		switch (type2) {
		case 0: // car
			setPlateParams(pVeh, pParams, 0);
			break;
		case 1: // bike
			setPlateParams(pVeh, pParams, 1);
			break;
		case 4: // heli
			setPlateParams(pVeh, pParams, 4);
			break;
		case 5: // plane
			setPlateParams(pVeh, pParams, 5);
			break;
		}

		break;

	case 2: // boat
		setPlateParams(pVeh, pParams, 2);
		break;
	}

}


__forceinline int __cdecl CLights__addSceneLight(int a1, int a2, int a3, float* a4, float* a5, float* a6, float* lightColor, float lightIntensity,
	int a9, int a10, float lightRadius, float a12, float a13, int a14, int a15, int a16) {
	return ((int(__cdecl*)
		(int a1, int a2, int a3, float* a4, float* a5, float* a6, float* lightColor, float lightIntensity, int a9, int a10, float lightRadius, float a12, float a13, int a14, int a15, int a16))
		g_CLights__addSceneLight)
		(a1, a2, a3, a4, a5, a6, lightColor, lightIntensity, a9, a10, lightRadius, a12, a13, a14, a15, a16);
}
struct VehicleTailLightsHook : CVehicle {
	static size_t ms_processTailLights;

	bool processTailLights(int a2, int a3, int a4, char a5, char a6, char a7, float a8, float a9, int a10, float* a11, char a12) {
		auto ret = ((bool(__thiscall*)(CVehicle*, int, int, int, char, char, char, float, float, int, float*, char))(ms_processTailLights))(
			this, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);

		bool b1 = (g_pfnGetVehicleFlags1_1(this) & 1) != 0;

		if (b1) {

			CVehExtParams::Params* pParams = g_extVehParams.getFromVehicle(this);

			auto pStruct = pParams->getStruct();

			for (size_t i = COMPONENT_PLATELIGHT_FIRST; i <= COMPONENT_PLATELIGHT_LAST; i++) {

				if (pStruct->m_aBones[i] != -1) {
					auto index = pStruct->m_aBones[i];
					Matrix34* pGlobalMtx = ((Matrix34 * (__thiscall*)(CVehicle*, int))g_CDynamicEntity__getGlobalMtx)(this, index);

					Vector3 vecColor = { 1.f, 1.f, 1.f };

					float f1 = 20.f;
					int i2 = 0;
					float f3 = 0.25;
					float f4 = 90.f;
					float f5 = 90.f;
					int i6 = 0;
					uint32_t i7 = g_pfnGetEntityInteriorHandle(this);
					uint8_t i8 = g_pfnGetEntityInteriorPortal(this);
					int i9 = 256;
					int i10 = 2;
					int i11 = 0;

					CLights__addSceneLight(i11, i10, i9, &pGlobalMtx->c.x, &pGlobalMtx->b.x, &pGlobalMtx->d.x, &vecColor.x, f1, i2, *g_CLights__m_pDefaultTxdID, f3, f4, f5, i7, i8, i6);
				}
			}
		}

		return ret;
	}

	static void init() {
		ms_processTailLights = setFnAddrInCallOpcode(g_hookAddr_processTailLights, getThisCallAddr(&processTailLights));
	}
};
size_t VehicleTailLightsHook::ms_processTailLights;

void CPlateFactory::init() {
	if (CConfig::ms_bPlateLight)
		VehicleTailLightsHook::init();

	if (CConfig::ms_bLicensePlates) {
		PlateTxds::init();
		ms_plateTexturesHashes.init();

		g_pfnVehFactoryAddon[g_numVehFactoryAddon++] = setStandartLicensePlate;

		g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnInit = initLicensePlates;
		g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnSetVars = setVarsLicensePlates;
		g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnUpdate = updateLicensePlates;
		g_numCustomVehFxAddons++;
	}

}


// native functions

int __cdecl setCarLicensePlateText0123(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pChars = params->getPlateInfo()->m_aLicensePlateText;
		//auto pChars = params->getPlateChars();


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

int __cdecl setCarLicensePlateText4567(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pChars = params->getPlateInfo()->m_aLicensePlateText;
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

int __cdecl setCarLicensePlateText891011(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pChars = params->getPlateInfo()->m_aLicensePlateText;
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


int __cdecl setCarLicensePlateTextTintUByte4(int32_t vehHandle, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pPlateInfo = params->getPlateInfo();
		pPlateInfo->m_vecPlateTextTint.x = r / 255.f;
		pPlateInfo->m_vecPlateTextTint.y = g / 255.f;
		pPlateInfo->m_vecPlateTextTint.z = b / 255.f;
		pPlateInfo->m_vecPlateTextTint.w = a / 255.f;

		return TRUE;
	}
	else
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_TINT_UBYTE4: veh handle %i out of bounds\n", vehHandle);

	return FALSE;
}

int __cdecl setCarLicensePlateTextTintFloat4(int32_t vehHandle, float r, float g, float b, float a) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pPlateInfo = params->getPlateInfo();
		pPlateInfo->m_vecPlateTextTint.x = r;
		pPlateInfo->m_vecPlateTextTint.y = g;
		pPlateInfo->m_vecPlateTextTint.z = b;
		pPlateInfo->m_vecPlateTextTint.w = a;

		return TRUE;
	}
	else
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_TINT_FLOAT4: veh handle %i out of bounds\n", vehHandle);

	return FALSE;
}

int __cdecl setCarLicensePlateTextLength(int32_t vehHandle, uint8_t length) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pPlateInfo = params->getPlateInfo();
		if (length >= 12) {
			PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_LENGTH: invalid length %i replaced by %i\n", length, 12);
			length = 12;
		}
		pPlateInfo->m_NumChars = length;


		return TRUE;
	}
	else
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_LENGTH: veh handle %i out of bounds\n", vehHandle);

	return FALSE;
}

int __cdecl setCarLicensePlateTextTexture(int32_t vehHandle, uint8_t texture) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pPlateInfo = params->getPlateInfo();
		if (texture >= CPlateFactory::MAX_NUM_PLATES_TEXTURES) {
			PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_TEXTURE : invalid texture %i replaced by %i\n", texture, 0);
			texture = 0;
		}
		pPlateInfo->m_LettersTexIndex = texture;

		return TRUE;
	}
	else
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_TEXTURE: veh handle %i out of bounds\n", vehHandle);

	return FALSE;
}

int __cdecl setCarLicensePlatePlateTexture(int32_t vehHandle, uint8_t texture) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pPlateInfo = params->getPlateInfo();
		if (texture >= CPlateFactory::MAX_NUM_PLATES_TEXTURES) {
			PRINT_DUBUG("SET_CAR_LICENSE_PLATE_PLATE_TEXTURE : invalid texture %i replaced by %i\n", texture, 0);
			texture = 0;
		}
		pPlateInfo->m_PlateTexIndex = texture;

		return TRUE;
	}
	else
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_PLATE_TEXTURE: veh handle %i out of bounds\n", vehHandle);

	return FALSE;
}

int __cdecl setCarLicensePlateTextScale(int32_t vehHandle, float x, float y) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pPlateInfo = params->getPlateInfo();
		pPlateInfo->m_vecLettersScaleAndOffset.x = x;
		pPlateInfo->m_vecLettersScaleAndOffset.y = y;

		return TRUE;
	}
	else
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_SCALE: veh handle %i out of bounds\n", vehHandle);

	return FALSE;
}

int __cdecl setCarLicensePlateTextOffset(int32_t vehHandle, float x, float y) {
	vehHandle = ((atPool<CVehicle>*)(*g_pCVehicle__ms_pPool))->indexOfHandle(vehHandle);
	if (vehHandle < g_extVehParams.m_size) {
		auto params = g_extVehParams.getByIndex(vehHandle);

		auto pPlateInfo = params->getPlateInfo();
		pPlateInfo->m_vecLettersScaleAndOffset.z = x;
		pPlateInfo->m_vecLettersScaleAndOffset.w = y;

		return TRUE;
	}
	else
		PRINT_DUBUG("SET_CAR_LICENSE_PLATE_TEXT_OFFSET: veh handle %i out of bounds\n", vehHandle);

	return FALSE;
}
