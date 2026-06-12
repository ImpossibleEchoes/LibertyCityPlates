#include "liveries.h"
#include "vehExtParams.h"
#include "utils.h"
#include "CVehicle.h"

#include <inttypes.h>
#include <vector>

struct tLiveryInfo {
	struct Info {
		std::vector<uint8_t> m_idxs;
		std::vector<Rect> m_rects;
	};
	std::vector<Info> m_Infos;
};

std::map<uint32_t, tLiveryInfo> g_Liveries;

void parseLiveriesIde(char* line) {
	char tokChars[4];
	strcpy(tokChars, " \t");

	auto tok = strtok(line, tokChars);

	PRINT_DUBUG("reading liveries for %s:", tok);

	auto namehash = getOrParseHash(tok);

	if (!g_Liveries.count(namehash)) {
		g_Liveries.insert(std::make_pair(namehash, tLiveryInfo()));
	}
	g_Liveries[namehash].m_Infos.push_back(tLiveryInfo::Info());
	tLiveryInfo::Info* pLiveries = &g_Liveries[namehash].m_Infos.back();

	tok = strtok(nullptr, tokChars);

	PRINT_DUBUG("\nidxs: ");

	int num = atoi(tok);
	pLiveries->m_idxs.resize(num);
	for (size_t i = 0; i < num; i++) {
		tok = strtok(nullptr, tokChars);
		pLiveries->m_idxs[i] = atoi(tok);
		PRINT_DUBUG(" %i", atoi(tok));
	}

	PRINT_DUBUG("\nzones: ");

	tok = strtok(nullptr, tokChars);
	num = atoi(tok);
	pLiveries->m_rects.resize(num);

	for (size_t i = 0; i < num; i++) {
		for (size_t j = 0; j < 4; j++) {
			tok = strtok(nullptr, tokChars);
			(&pLiveries->m_rects[i].minX)[j] = atof(tok);
		}
		PRINT_DUBUG(" %f %f %f %f\t", pLiveries->m_rects[i].minX, pLiveries->m_rects[i].maxX, pLiveries->m_rects[i].minY, pLiveries->m_rects[i].maxY);
	}

	PRINT_DUBUG("\n");



}

void __cdecl setLivery(CVehicle* pVeh, CVehExtParams::Params* pParams) {
	auto mdl = g_modelPointers[pVeh->getModelIndex()];
	auto it = g_Liveries.find(mdl->m_hash);
	if (it != g_Liveries.end()) {


		auto pMtx = pVeh->getTransform();
		for (size_t i = 0; i < it->second.m_Infos.size(); i++) {
			for (const auto& rect : it->second.m_Infos[i].m_rects) {
				if (pMtx->d.x > rect.minX && pMtx->d.x < rect.maxX && pMtx->d.y > rect.minY && pMtx->d.y < rect.maxY) {
					auto idx = getRandomElementFromVector(it->second.m_Infos[i].m_idxs);
					if (idx < mdl->m_NumLiveries) {
						((uint8_t(__thiscall*)(CVehicle *, int))(g_CVehicle__setLivery))(pVeh, idx);
						((uint8_t(__thiscall*)(CVehicle *))(g_CVehicle__applyCustomShaderEffectChanges))(pVeh);
						PRINT_DUBUG("used livery %i\n", idx);
					}
					else
						PRINT_DUBUG("index %i out of bounds(%i)\n", idx, mdl->m_NumLiveries);
					return;


				}

			}


		}
		PRINT_DUBUG("livery not changed\n");
	}

}

void initLiveries() {
	//if (CConfig::ms_bLicensePlates) {

	g_pfnVehFactoryAddon[g_numVehFactoryAddon++] = setLivery;

	//}

}