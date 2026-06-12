#include "weapon.h"

#include "addrs.h"
#include "hookFns.h"
#include "utils.h"
#include "trace.h"
#include "stdio.h"
#include <io.h>
#include <cstdio>
#include "newComponents.h"
#include "CVehicle.h"
#include "crSkeleton.h"
#include "helpers.h"
#include "licensePlates.h"
#include "liveries.h"

std::map<uint32_t, CCarWeapInfo*> g_carWeapInfo;

CWeapon::CWeapon() { ((void(__thiscall*)(CWeapon*))(g_CWeapon__constructor))(this); }
CWeapon::~CWeapon() { ((void(__thiscall*)(CWeapon*))(g_CWeapon__destructor))(this); }


void CWeapon::initialize(uint32_t weapId, uint32_t numAmmo, uint8_t unk) {
	((void(__thiscall*)(CWeapon*, uint32_t weapId, uint32_t numAmmo, uint8_t unk))(g_CWeapon__init))(this, weapId, numAmmo, unk);
}
bool CWeapon::fireSingleBullet(CEntity* who, Matrix34* where, Vector3* whereOverride, Vector3* velocityOverride, bool unk, uint32_t unk2, uint32_t unk3, int unk4, float damageOverride) {
	return ((bool(__thiscall*)(CWeapon*, CEntity * who, Matrix34 * where, Vector3 * whereOverride, Vector3 * velocityOverride, bool unk, uint32_t unk2, uint32_t unk3, int unk4, float damageOverride))(g_CWeapon__fire))
		(this, who, where, whereOverride, velocityOverride, unk, unk2, unk3, unk4, damageOverride);
}

void CWeapon::update(CEntity* pEntity) {
	((void(__thiscall*)(CWeapon*, CEntity * pEntity))(g_CWeapon__update))(this, pEntity);
}


size_t g_readIde_origcall;

void trim(char* str) {
	size_t len = strlen(str);

	while (len > 0) {
		char last_char = str[len - 1];

		if (last_char == '\n' || last_char == '\r') {
			str[len - 1] = '\0';
			len--;
		}
		else
			break;
	}
}

void parseCarWeap(char* line) {
	char tokChars[4];
	strcpy(tokChars, " \t");

	auto tok = strtok(line, tokChars);

	PRINT_DUBUG("reading weapons for %s\n", tok);

	uint32_t namehash = getOrParseHash(tok);

	CCarWeapInfo* info;
	if (g_carWeapInfo.count(namehash))
		info = g_carWeapInfo[namehash];
	else
		info = new CCarWeapInfo;
	
	tok = strtok(nullptr, tokChars);
	auto slot = &info->m_aWeapons[atoi(tok)];

	tok = strtok(nullptr, tokChars);
	//auto hash = jenkins_one_at_a_time_hash(tok);
	uint32_t hash = getOrParseHash(tok);


	slot->m_weapId = ((int (__cdecl*)(int, int))(g_getWeaponInfoIdByName))(hash, -1);
	if (slot->m_weapId == -1) {
		PRINT_DUBUG("invalid weapon name %s\n", tok);
		if (!g_carWeapInfo.count(namehash))
			delete info;
		return;
	}
	else
		PRINT_DUBUG("weap id %i\n", slot->m_weapId);
	slot->m_weapNameHash = hash;

	tok = strtok(nullptr, tokChars);
	slot->m_numBones = atoi(tok);
	int boneIndex = 0;
	for (size_t i = 0; i < slot->m_numBones; i++) {
		tok = strtok(nullptr, tokChars);
		if (!strcmp(tok, "weapon_a"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_A;
		else if (!strcmp(tok, "weapon_b"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_B;
		else if (!strcmp(tok, "weapon_c"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_C;
		else if (!strcmp(tok, "weapon_d"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_D;
		else if (!strcmp(tok, "weapon_e"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_E;
		else if (!strcmp(tok, "weapon_f"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_F;
		else if (!strcmp(tok, "weapon_g"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_G;
		else if (!strcmp(tok, "weapon_h"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_H;
		else if (!strcmp(tok, "weapon_i"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_I;
		else if (!strcmp(tok, "weapon_j"))
			slot->m_aBones[boneIndex] = COMPONENT_WEAPON_J;
		else {
			PRINT_DUBUG("invalid bone %s\n", tok);
			continue;
		}
		PRINT_DUBUG("added bone %s at %i\n", tok, boneIndex);
		boneIndex++;

	}
	slot->m_numBones = boneIndex;
	if (slot->m_numBones < 1) {
		PRINT_DUBUG("no bones\n");
		if (!g_carWeapInfo.count(namehash))
			delete info;
		return;
	}

	if (!g_carWeapInfo.count(namehash))
		g_carWeapInfo.insert(std::make_pair(namehash, info));


}

void exportCarWeap(FILE* f) {
	for (auto& entry : g_carWeapInfo) {
		for (size_t i = 0; i < 3; i++) {
			if (entry.second->m_aWeapons[i].m_weapId == -1)
				continue;
			fprintf(f, "hash:%i %i hash:%i %i", entry.first, i, entry.second->m_aWeapons[i].m_weapNameHash, entry.second->m_aWeapons[i].m_numBones);
			for (size_t j = 0; j < entry.second->m_aWeapons[i].m_numBones; j++) {
				// ToDo: make switch
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_A)
					fprintf(f, " %s", "weapon_a");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_B)
					fprintf(f, " %s", "weapon_b");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_C)
					fprintf(f, " %s", "weapon_c");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_D)
					fprintf(f, " %s", "weapon_d");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_E)
					fprintf(f, " %s", "weapon_e");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_F)
					fprintf(f, " %s", "weapon_f");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_G)
					fprintf(f, " %s", "weapon_g");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_H)
					fprintf(f, " %s", "weapon_h");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_I)
					fprintf(f, " %s", "weapon_i");
				if(entry.second->m_aWeapons[i].m_aBones[j] == COMPONENT_WEAPON_J)
					fprintf(f, " %s", "weapon_j");

			}
			fprintf(f, "\n");

		}

	}
}

template <bool _ReadIde> int __cdecl readIde(char *path, char* mode) {
	auto hRageFile = ((void* (__cdecl*)(char*, char*))(g_gta_fopen))(path, mode);
	
	int type = 0;

	if (hRageFile) {
		HANDLE hFile = ((HANDLE*)hRageFile)[1];

		if (hFile != INVALID_HANDLE_VALUE) {
			int fd = _open_osfhandle((intptr_t)hFile, 0);
			if (fd != -1) {
				FILE* f = _fdopen(fd, "r");
				if (f == nullptr)
					_close(fd);
				else {
					char line[0x1FF];
					while (fgets(line, sizeof(line), f) != nullptr) {
						trim(line);
						if (line[0] && line[0] != '#') {
							if (type) {
								if (line[0] == 'e' && line[1] == 'n' && line[2] == 'd' && line[3] == '\0')
									type = 0;
								else {
									switch (type) {
									case 1:
										parseCarWeap(line);
										break;
									case 2:
										CPlateFactory::parseCarPlatesIde(line);
										break;
									case 3:
										CPlateFactory::parseLicensePlatesIde(line);
										break;
									case 4:
										parseLiveriesIde(line);
										break;
									}
								}
							}
							else if (line[0] == 'c' && line[1] == 'a' && line[2] == 'r' && line[3] == 'w' && line[4] == 'e' && line[5] == 'a' && line[6] == 'p')
								type = 1;
							else if (line[0] == 'u' && line[1] == 's' && line[2] == 'e' && line[3] == 'd' && line[4] == '_' && line[5] == 'p' && line[6] == 'l' && line[7] == 'a' && line[8] == 't' && line[9] == 'e' && line[10] == 's')
								type = 2;
							else if (line[0] == 'l' && line[1] == 'i' && line[2] == 'c' && line[3] == 'e' && line[4] == 'n' && line[5] == 's' && line[6] == 'e' && line[7] == '_' && line[8] == 'p' && line[9] == 'l' && line[10] == 'a' && line[11] == 't' && line[12] == 'e' && line[13] == 's')
								type = 3;
							else if (line[0] == 'a' && line[1] == 'r' && line[2] == 'e' && line[3] == 'a' && line[4] == '_' && line[5] == 'l' && line[6] == 'i' && line[7] == 'v' && line[8] == 'e' && line[9] == 'r' && line[10] == 'i' && line[11] == 'e' && line[12] == 's')
								type = 4;
						}
					}
				}
				fclose(f);
			}
		}
		((void(__cdecl*)(void*))(g_gta_fclose))(hRageFile);
	}

	if constexpr (_ReadIde)
		return ((int(__cdecl*)(char*, char*))(g_readIde_origcall))(path, mode);
	else
		return 0;

}


void readIdeData() {
	PRINT_DUBUG("reading data\n");

	char buf[0x20];
	strcpy(buf, "exportedData.ide");
	int end = strlen(buf) + 1;
	strcpy(buf + end, "r");

	CPlateFactory::clearUsedPlates();

	readIde<false>(buf, buf + end);
}

void dumpIdeData() {
	PRINT_DUBUG("dumping data\n");
	FILE* f = fopen("exportedData.ide", "w");


	fprintf(f, "license_plates\n");
	CPlateFactory::exportLicensePlatesIde(f);
	fprintf(f, "end\n\n");

	fprintf(f, "used_plates\n");
	CPlateFactory::exportCarPlatesIde(f);
	fprintf(f, "end\n\n");

	fprintf(f, "carweap\n");
	exportCarWeap(f);
	fprintf(f, "end\n");

	fclose(f);
}


namespace car_weapons {

struct ioValue {
	struct __declspec(align(4)) History {
		uint8_t m_nbValue;
		void* _f4; // unk ptr
	};

	uint8_t _f4;
	uint8_t _f5;
	uint8_t m_nbValue;
	uint8_t m_nbLastValue;
	uint8_t m_nbHistoryIndex;
	uint8_t _f9;
	uint8_t _fa;
	uint8_t _fb;
	History* m_pHistory; // => num = 0x40

	__forceinline bool isPressed() {
		return (_f4 ^ m_nbValue) > 0x3F;
	}

	__forceinline bool isJustPressed() {
		return m_nbLastValue <= 0x3F && isPressed();
	}

	__forceinline virtual ~ioValue() {} // только один виртуальный метод
};

struct CPad {
	BYTE __0[0x2698];
	ioValue m_aValues[187];
};

enum ePadControls {
	PADCONTROL_VEH_GUN_LEFT = 34,
	PADCONTROL_VEH_GUN_RIGHT,
	PADCONTROL_VEH_GUN_UP,
	PADCONTROL_VEH_GUN_DOWN,
	PADCONTROL_VEH_ATTACK,
	PADCONTROL_VEH_ATTACK2,
	PADCONTROL_MOUSE_UD = 88,
	PADCONTROL_MOUSE_LR,
};

bool isVehAttackPressed(CPed* pPed) {
	CPad* pad = ((CPad * (__thiscall*)(void* pThis))(g_CPed__getPad2))(pPed);
	if (pad)
		return (pad->m_aValues[PADCONTROL_VEH_ATTACK]._f4 ^ pad->m_aValues[PADCONTROL_VEH_ATTACK].m_nbValue) > 0x7F;
	else
		return false;
}


float calculateTurretOrientation(Vector3* pVec, float old, float speed) {

	float targetAngle = atan2f(-pVec->x, pVec->y);

	float angleDelta = targetAngle - old;

	float normalizedDelta = normalizeAngleSafeWithClamp25(angleDelta);

	float maxRotation = *(float*)g_pfTimeStep * speed;
	float minRotation = -maxRotation;

	float actualRotation;

	if (normalizedDelta < minRotation)
		actualRotation = minRotation;
	else if (normalizedDelta > maxRotation)
		actualRotation = maxRotation;
	else
		actualRotation = normalizedDelta;

	return normalizeAngleSafeWithClamp25(old + actualRotation);
}

float calculateTurretElevation(Vector3* pVec, float old, float elevationSpeed, float angleOffset) {
	float horizontalDistance = sqrtf((pVec->x * pVec->x) + (pVec->y * pVec->y));

	float pureTargetElevation = atan2f(pVec->z, horizontalDistance);

	float finalTargetElevation = pureTargetElevation + angleOffset;

	float currentElevation = old;
	float maxDelta = *(float*)g_pfTimeStep * elevationSpeed;

	float delta = finalTargetElevation - currentElevation;

	if (delta > maxDelta)
		return currentElevation + maxDelta;
	else if (delta < -maxDelta)
		return currentElevation - maxDelta;
	else
		return finalTargetElevation;
}

struct CAutomobile : CVehicle {
	
	static size_t ms_processAutomobile_origcall;
	static size_t ms_processHeli_origcall;
	static size_t ms_processPlane_origcall;

	void processTurret() {
		auto pDriver = g_pfnGetDriver(this);
		if (!pDriver)
			return;
		else if (!((bool(__thiscall*)(void*))(g_CPed__getIsPlayer))(pDriver))
			return;
		char vehicleFlags1_0 = g_pfnGetVehFlags1_0(this);
		if ((vehicleFlags1_0 & 8) == 0)
			return;

		//{
		//	auto pMdl = g_modelPointers[this->getModelIndex()];
		//	printf("%p\n", &pMdl->m_pDrawableRef->pDrawable->m_pShaderGroup->m_ppShaders[0]->m_instanceData._f1c);
		//}

		auto pSkel = ((crSkeletonData * (__thiscall*)(CVehicle * _a))(g_CDynamicEntity__getSkeletonData))(this);

		auto params = g_extVehParams.getFromVehicle(this);

		auto pStruct = params->getStruct();

		auto weapons = params->getCarWeapInfoInfo();

		if (pStruct->m_aBones[COMPONENT_TURRET_1BARREL] || pStruct->m_aBones[COMPONENT_TURRET_1BASE] ||
			pStruct->m_aBones[COMPONENT_TURRET_2BARREL] || pStruct->m_aBones[COMPONENT_TURRET_2BASE]) {

			auto pUnkObject = g_unkObject[3];

			float xAngleOffset = 0.f;
			Matrix34* pCamMatrix = nullptr;

			if (pUnkObject != nullptr) {
				auto pCamera = ((uint8_t * (__thiscall*)(void*, int a2, int a3))(g_unkCall))(pUnkObject, 2, 0);

				if (pCamera) {

					if (pCamera[556] == 1)
						xAngleOffset = 0.087266468;
					else if (pCamera[556] == 2 || pCamera[556] == 3)
						xAngleOffset = 0.043633234;

					pCamMatrix = (Matrix34*)(pCamera + 16);


				}
			}

			if (!pCamMatrix)
				pCamMatrix = g_unkMatrix34;

			Matrix34 cannonViewMtx = *pCamMatrix;

			Vector3 vec;

			((Vector3 * (__thiscall*)(Matrix34*, Vector3*))(g_Matrix34__getEuler))(&cannonViewMtx, &vec);

			if (fabsf(vec.x) > 1.2217305)
				((void(__thiscall*)(Matrix34*, float))(g_Matrix34__rotateLocalX))(&cannonViewMtx, sign(vec.x) * 1.2217305 - vec.x);

			Vector3 aimPointLocal{ 0.f, 1000.f, 0.f };

			Vector3 aimPointWorld{ (cannonViewMtx.a.x * aimPointLocal.x) + (cannonViewMtx.b.x * aimPointLocal.y) + (cannonViewMtx.c.x * aimPointLocal.z) + cannonViewMtx.d.x,
				(cannonViewMtx.a.y * aimPointLocal.x) + (cannonViewMtx.b.y * aimPointLocal.y) + (cannonViewMtx.c.y * aimPointLocal.z) + cannonViewMtx.d.y,
				(cannonViewMtx.a.z * aimPointLocal.x) + (cannonViewMtx.b.z * aimPointLocal.y) + (cannonViewMtx.c.z * aimPointLocal.z) + cannonViewMtx.d.z };



			if (pStruct->m_aBones[COMPONENT_TURRET_1BASE] != -1) {
				auto pCoord = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_1BASE]);

				auto mtx = *pCoord;

				auto pBone = pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TURRET_1BASE]].m_pParent;
				while (pBone) {
					auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pBone->m_wBoneIndex);
					mtx.mult(pMtx);
					pBone = pBone->m_pParent;
				}
				mtx.identityRot();
				auto pMtx = getTransform();
				mtx.mult(pMtx);
				pCoord = &mtx;

				Vector3 vec2{ aimPointWorld.x - pCoord->d.x, aimPointWorld.y - pCoord->d.y, aimPointWorld.z - pCoord->d.z };
				Vector3 vec3{ ((pCoord->a.y * vec2.y) + (vec2.x * pCoord->a.x)) + (pCoord->a.z * vec2.z),
					((pCoord->b.y * vec2.y) + (vec2.x * pCoord->b.x)) + (pCoord->b.z * vec2.z),
					((pCoord->c.y * vec2.y) + (pCoord->c.x * vec2.x)) + (pCoord->c.z * vec2.z) };

				weapons->m_fTurret1Orientation = calculateTurretOrientation(&vec3, weapons->m_fTurret1Orientation, 1.f);

				pBone = &pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TURRET_1BASE]];
				if (weapons->m_fTurret1Orientation > pBone->m_vRotMax.z)
					weapons->m_fTurret1Orientation = pBone->m_vRotMax.z;
				else if (weapons->m_fTurret1Orientation < pBone->m_vRotMin.z)
					weapons->m_fTurret1Orientation = pBone->m_vRotMin.z;

				pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_1BASE]);
				pMtx->identityRot();
				pMtx->a.x = cosf(-weapons->m_fTurret1Orientation);
				pMtx->a.y = -sinf(-weapons->m_fTurret1Orientation);
				pMtx->b.x = sinf(-weapons->m_fTurret1Orientation);
				pMtx->b.y = cosf(-weapons->m_fTurret1Orientation);
			}

			if (pStruct->m_aBones[COMPONENT_TURRET_1BARREL] != -1) {
				auto pCoord = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_1BARREL]);

				auto mtx = *pCoord;

				auto pBone = pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TURRET_1BARREL]].m_pParent;
				while (pBone) {
					auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pBone->m_wBoneIndex);
					mtx.mult(pMtx);
					pBone = pBone->m_pParent;
				}
				mtx.identityRot();
				auto pMtx = getTransform();
				mtx.mult(pMtx);
				pCoord = &mtx;

				Vector3 vec2{ aimPointWorld.x - pCoord->d.x, aimPointWorld.y - pCoord->d.y, aimPointWorld.z - pCoord->d.z };
				Vector3 vec3{ ((pCoord->a.y * vec2.y) + (vec2.x * pCoord->a.x)) + (pCoord->a.z * vec2.z),
					((pCoord->b.y * vec2.y) + (vec2.x * pCoord->b.x)) + (pCoord->b.z * vec2.z),
					((pCoord->c.y * vec2.y) + (pCoord->c.x * vec2.x)) + (pCoord->c.z * vec2.z) };

				weapons->m_fTurret1Elevation = calculateTurretElevation(&vec3, weapons->m_fTurret1Elevation, 1.f, xAngleOffset);
				pBone = &pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TURRET_1BARREL]];
				if (weapons->m_fTurret1Elevation > pBone->m_vRotMax.x)
					weapons->m_fTurret1Elevation = pBone->m_vRotMax.x;
				else if (weapons->m_fTurret1Elevation < pBone->m_vRotMin.x)
					weapons->m_fTurret1Elevation = pBone->m_vRotMin.x;


				pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_1BARREL]);
				pMtx->identityRot();
				pMtx->b.y = cosf(-weapons->m_fTurret1Elevation);
				pMtx->b.z = -sinf(-weapons->m_fTurret1Elevation);
				pMtx->c.y = sinf(-weapons->m_fTurret1Elevation);
				pMtx->c.z = cosf(-weapons->m_fTurret1Elevation);
			}

			if (pStruct->m_aBones[COMPONENT_TURRET_2BASE] != -1) {
				auto pCoord = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_2BASE]);

				auto mtx = *pCoord;

				auto pBone = pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TURRET_2BASE]].m_pParent;
				while (pBone) {
					auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pBone->m_wBoneIndex);
					mtx.mult(pMtx);
					pBone = pBone->m_pParent;
				}
				mtx.identityRot();
				auto pMtx = getTransform();
				mtx.mult(pMtx);
				pCoord = &mtx;

				Vector3 vec2{ aimPointWorld.x - pCoord->d.x, aimPointWorld.y - pCoord->d.y, aimPointWorld.z - pCoord->d.z };
				Vector3 vec3{ ((pCoord->a.y * vec2.y) + (vec2.x * pCoord->a.x)) + (pCoord->a.z * vec2.z),
					((pCoord->b.y * vec2.y) + (vec2.x * pCoord->b.x)) + (pCoord->b.z * vec2.z),
					((pCoord->c.y * vec2.y) + (pCoord->c.x * vec2.x)) + (pCoord->c.z * vec2.z) };

				weapons->m_fTurret2Orientation = calculateTurretOrientation(&vec3, weapons->m_fTurret2Orientation, 1.f);

				pBone = &pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TURRET_2BASE]];
				if (weapons->m_fTurret2Orientation > pBone->m_vRotMax.z)
					weapons->m_fTurret2Orientation = pBone->m_vRotMax.z;
				else if (weapons->m_fTurret2Orientation < pBone->m_vRotMin.z)
					weapons->m_fTurret2Orientation = pBone->m_vRotMin.z;

				pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_2BASE]);
				pMtx->identityRot();
				pMtx->a.x = cosf(-weapons->m_fTurret2Orientation);
				pMtx->a.y = -sinf(-weapons->m_fTurret2Orientation);
				pMtx->b.x = sinf(-weapons->m_fTurret2Orientation);
				pMtx->b.y = cosf(-weapons->m_fTurret2Orientation);
			}

			if (pStruct->m_aBones[COMPONENT_TURRET_2BARREL] != -1) {
				auto pCoord = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_2BARREL]);

				auto mtx = *pCoord;

				auto pBone = pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TURRET_2BARREL]].m_pParent;
				while (pBone) {
					auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pBone->m_wBoneIndex);
					mtx.mult(pMtx);
					pBone = pBone->m_pParent;
				}
				mtx.identityRot();
				auto pMtx = getTransform();
				mtx.mult(pMtx);
				pCoord = &mtx;

				Vector3 vec2{ aimPointWorld.x - pCoord->d.x, aimPointWorld.y - pCoord->d.y, aimPointWorld.z - pCoord->d.z };
				Vector3 vec3{ ((pCoord->a.y * vec2.y) + (vec2.x * pCoord->a.x)) + (pCoord->a.z * vec2.z),
					((pCoord->b.y * vec2.y) + (vec2.x * pCoord->b.x)) + (pCoord->b.z * vec2.z),
					((pCoord->c.y * vec2.y) + (pCoord->c.x * vec2.x)) + (pCoord->c.z * vec2.z) };

				weapons->m_fTurret2Elevation = calculateTurretElevation(&vec3, weapons->m_fTurret2Elevation, 1.f, xAngleOffset);
				pBone = &pSkel->m_pBones[pStruct->m_aBones[COMPONENT_TURRET_2BARREL]];
				if (weapons->m_fTurret2Elevation > pBone->m_vRotMax.x)
					weapons->m_fTurret2Elevation = pBone->m_vRotMax.x;
				else if (weapons->m_fTurret2Elevation < pBone->m_vRotMin.x)
					weapons->m_fTurret2Elevation = pBone->m_vRotMin.x;

				pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_2BARREL]);
				pMtx->identityRot();
				pMtx->b.y = cosf(-weapons->m_fTurret2Elevation);
				pMtx->b.z = -sinf(-weapons->m_fTurret2Elevation);
				pMtx->c.y = sinf(-weapons->m_fTurret2Elevation);
				pMtx->c.z = cosf(-weapons->m_fTurret2Elevation);
			}
		}

		if (isVehAttackPressed(pDriver)) {

			for (size_t i = 0; i < MAX_NUM_CAR_WEAPONS; i++) {
				if (weapons->m_slots[i].m_bActive && weapons->m_slots[i].m_pWeapon) {
					weapons->m_slots[i].m_pWeapon->update((::CEntity*)this);

					auto curIndex = pStruct->m_aBones[weapons->m_slots[i].m_aBones[weapons->m_slots[i].m_CurBone]];

					Matrix34 mtx;
					auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, curIndex);

					mtx = *pMtx;

					auto pBone = pSkel->m_pBones[curIndex].m_pParent;
					while (pBone) {
						pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pBone->m_wBoneIndex);
						mtx.mult(pMtx);
						pBone = pBone->m_pParent;
					}
					pMtx = getTransform();
					mtx.mult(pMtx);

					if (weapons->m_slots[i].m_pWeapon->fireSingleBullet((::CEntity*)this, &mtx, 0, 0, 0, 0, 0, 0, -1.f)) {
						weapons->m_slots[i].m_CurBone++;
						if (weapons->m_slots[i].m_CurBone >= weapons->m_slots[i].m_NumBones)
							weapons->m_slots[i].m_CurBone = 0;
					}

				}

			}
		}
	}

	void processAutomobile() {
		((void(__thiscall*)(CAutomobile*))(ms_processAutomobile_origcall))(this);
		processTurret();
	}

	void processHeli() {
		((void(__thiscall*)(CAutomobile*))(ms_processHeli_origcall))(this);
		processTurret();
	}

	void processPlane() {
		((void(__thiscall*)(CAutomobile*))(ms_processPlane_origcall))(this);
		processTurret();
	}

	static void initPatch() {
		CAutomobile::ms_processAutomobile_origcall = writeDWORD(g_vmtAddr_CAutomobile__processCannon, getThisCallAddr(&processAutomobile));
		CAutomobile::ms_processHeli_origcall = writeDWORD(g_vmtAddr_CAutomobile__processCannon, getThisCallAddr(&processHeli));
		CAutomobile::ms_processPlane_origcall = writeDWORD(g_vmtAddr_CAutomobile__processCannon, getThisCallAddr(&processPlane));

	}
};

size_t CAutomobile::ms_processAutomobile_origcall;
size_t CAutomobile::ms_processHeli_origcall;
size_t CAutomobile::ms_processPlane_origcall;

size_t g_pedShotBlocking_origcall;

bool __cdecl pedShotBlocking(CPed* pPed) {

	if (auto veh = g_pfnGetVehicle(pPed)) {
		if (auto params = g_extVehParams.getFromVehicle(veh)) {
			auto weapons = params->getCarWeapInfoInfo();
			for (size_t i = 0; i < sizeof weapons->m_slots / sizeof * weapons->m_slots; i++)
				if (weapons->m_slots[i].m_bActive)
					return false;
		}
	}

	return ((bool(__cdecl*)(CPed*))(g_pedShotBlocking_origcall))(pPed);
}

void __cdecl createVehicleWeapon(CVehicle* _a, CVehExtParams::Params* pParams) {
	auto mdl = g_modelPointers[_a->getModelIndex()];
	if (g_carWeapInfo.count(mdl->m_hash)) {
		pParams->getCarWeapInfoInfo()->initialize(g_carWeapInfo[mdl->m_hash]);

	}
}


void init() {
	CAutomobile::initPatch();
	g_pedShotBlocking_origcall = setFnAddrInCallOpcode(g_hookAddr_pedShotBlocking, (size_t)pedShotBlocking);
	g_pfnVehFactoryAddon[g_numVehFactoryAddon++] = createVehicleWeapon;

}
}



void initWeapons() {
	car_weapons::init();
	g_readIde_origcall = setFnAddrInCallOpcode(g_hookAddr_readIde, (size_t)readIde<true>);

}