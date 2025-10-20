#pragma once
#include <inttypes.h>
#include "rage_math.h"

#include <map>

#define MAX_NUM_CAR_WEAPONS 3
#define MAX_NUM_CAR_WEAPONS_BONES 10

struct CCarWeapInfo {
	struct Weapon {
		uint32_t m_weapId;
		uint32_t m_numBones;
		uint32_t m_aBones[MAX_NUM_CAR_WEAPONS_BONES];

		Weapon() {
			m_weapId = -1;
			m_numBones = 0;
		}
	};
	
	Weapon m_aWeapons[MAX_NUM_CAR_WEAPONS];
};

extern std::map<uint32_t, CCarWeapInfo*> g_carWeapInfo;

struct CEntity;

struct alignas(4) CWeapon {
	uint8_t m_weapAud[0x14];
	uint32_t field_14;
	uint32_t m_dwInfoIndex;
	uint32_t field_1C;
	uint32_t m_dwTimer;
	uint8_t field_24;
	uint32_t field_28;
	uint32_t field_2C;
	uint32_t field_30;
	uint32_t field_34;
	uint32_t field_38;
	uint32_t field_3C;
	uint32_t field_40;
	uint32_t field_44;
	uint32_t field_48;
	uint32_t field_4C;
	uint8_t field_50;
	uint32_t field_54;
	uint8_t field_58;

	// ammo in CE
	uint32_t m_secbuffArray1;
	uint32_t m_secbuffArray2;

	CWeapon();
	~CWeapon();

	void initialize(uint32_t weapId, uint32_t numAmmo, uint8_t unk);
	bool fireSingleBullet(CEntity * who, Matrix34* where, Vector3* whereOverride, Vector3* velocityOverride, bool unk, uint32_t unk2, uint32_t unk3, int unk4, float damageOverride);
	void update(CEntity* pEntity);
};

void initWeapons();