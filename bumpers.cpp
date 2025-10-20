#include "bumpers.h"

#include "CHandling.h"
#include "addrs.h"
#include "config.h"
#include "fragType.h"
#include "helpers.h"
#include "hookFns.h"

namespace bumpers {

struct fragType {
	uint8_t __0[0xD0];
	fragTypeGroup** m_ppGroups;
};


struct CVehicleStruct {
	int32_t m_componentBones[99];
};

struct CVehicleModelInfo {
	uint8_t __0[0x8];
	fragType* m_pFragType;
	uint8_t __C[0x60];
	uint32_t m_dwVehicleType;
	uint8_t __70[0x1C];
	uint32_t m_dwHandlingIndex;
	uint8_t __90[0x3C];
	CVehicleStruct* m_pStruct;

	static size_t ms_setVehiclePhysics_origcall;

	char setVehiclePhysicsUsedMass() {
		auto result = ((char(__thiscall*)(CVehicleModelInfo*))(ms_setVehiclePhysics_origcall))(this);
		if (!m_dwVehicleType) {
			if (m_pStruct) {

				float mass = g_handling[m_dwHandlingIndex].m_fMass;

				for (size_t i = 35; i < 39; i++) {
					auto boneIndex = m_pStruct->m_componentBones[i];
					if (boneIndex != -1) {
						int32_t groupIndex = ((int32_t(__cdecl*)(fragType*, int32_t))(g_fragType__getGroupIndexByBoneIndex))(m_pFragType, boneIndex);

						if (groupIndex != -1) {
							auto pGroup = m_pFragType->m_ppGroups[groupIndex];


							pGroup->m_fForceTransmissionScaleUp = 0.f; // Наскільки ефективно сила, отримана бампером, передається на кузов машини
							pGroup->m_fForceTransmissionScaleDown = 0.f; // Наскільки сила, отримана кузовом, передається на бампер

							pGroup->m_fStrength = mass * CConfig::ms_fBumperStrength;
							if (pGroup->m_fStrength < CConfig::ms_fMinBumperStrength)
								pGroup->m_fStrength = CConfig::ms_fMinBumperStrength;

							groupIndex = pGroup->m_nbChildGroupsPointersIndex;
							if (groupIndex - 1 <= 0xFD) {
								pGroup = m_pFragType->m_ppGroups[groupIndex];
								pGroup->m_fStrength = -1.f;
								pGroup->m_fForceTransmissionScaleUp = 0.f;
								pGroup->m_fForceTransmissionScaleDown = 0.f;
							}

						}
					}
				}
			}
		}
		return result;
	}

	char setVehiclePhysics() {
		auto result = ((char(__thiscall*)(CVehicleModelInfo*))(ms_setVehiclePhysics_origcall))(this);
		if (!m_dwVehicleType) {
			if (m_pStruct) {

				float mass = g_handling[m_dwHandlingIndex].m_fMass;
				for (size_t i = 35; i < 39; i++) {
					auto boneIndex = m_pStruct->m_componentBones[i];
					if (boneIndex != -1) {
						int32_t groupIndex = ((int32_t(__cdecl*)(fragType*, int32_t))(g_fragType__getGroupIndexByBoneIndex))(m_pFragType, boneIndex);

						if (groupIndex != -1) {
							auto pGroup = m_pFragType->m_ppGroups[groupIndex];

							pGroup->m_fForceTransmissionScaleUp = 0.f; // Наскільки ефективно сила, отримана бампером, передається на кузов машини
							pGroup->m_fForceTransmissionScaleDown = 0.f; // Наскільки сила, отримана кузовом, передається на бампер

							pGroup->m_fStrength = CConfig::ms_fBumperStrength;

							groupIndex = pGroup->m_nbChildGroupsPointersIndex;
							if (groupIndex - 1 <= 0xFD) {
								pGroup = m_pFragType->m_ppGroups[groupIndex];
								pGroup->m_fStrength = -1.f;
								pGroup->m_fForceTransmissionScaleUp = 0.f;
								pGroup->m_fForceTransmissionScaleDown = 0.f;
							}
						}
					}
				}
				//PRINT_DUBUG("%s\t%f\n", (char*)((uint8_t*)this + 0x60), CConfig::ms_fBumperStrength);
			}
		}
		return result;
	}
};

size_t CVehicleModelInfo::ms_setVehiclePhysics_origcall = 0;

struct phBound {
	uint8_t __0[0x94];
	uint32_t* _f94; // see more in ida database

};

struct phArchetype {
	uint8_t __0[0xC];

	phBound* m_pBound;
};

struct fragInst {
	uint8_t __0[0x4];
	phArchetype* m_pArchetype;
};

struct CVeh {

	static size_t ms_origcall;

	char setVehComponentsColFalgs() {
		//printf("setVehComponentsColFalgs()\n");

		auto result = ((char(__thiscall*)(CVeh*))(ms_origcall))(this);

		if (!g_pfnGetVehMainType((CVehicle*)this) && !g_pfnGetVehSpecialType((CVehicle*)this)) {

			auto pFragInst = (fragInst*)g_pfnGetVehFragInst((CVehicle*)this);
			auto pBound = pFragInst->m_pArchetype->m_pBound;

			if (pBound->_f94) {
				auto modelIndex = *(WORD*)((uint8_t*)this + 0x2E);
				for (size_t i = 35; i < 39; i++) {
					auto boneIndex = ((CVehicleModelInfo*)g_modelPointers[modelIndex])->m_pStruct->m_componentBones[i];
					if (boneIndex != -1) {
						auto pFragType = ((fragType * (__thiscall*)(fragInst*))(g_fragInstGta__getFragType))(pFragInst);

						int32_t childIndex = ((int32_t(__cdecl*)(fragType*, int32_t))(g_fragType__getChildIndexByBoneIndex))(pFragType, boneIndex);

						if (childIndex != -1)
							pBound->_f94[2 * childIndex + 1] = 0x7BFE;

					}
				}
			}
		}

		return result;

	}
};
size_t CVeh::ms_origcall = 0;



}

void initBumpers() {

	if (CConfig::ms_fBumperStrength > 0.f) {
		bumpers::CVeh::ms_origcall = writeDWORD(g_vmtAddr_CVehicle__setVehComponentsColFalgs, getThisCallAddr(&bumpers::CVeh::setVehComponentsColFalgs));

		if (CConfig::ms_bBumperStrengthIsMassMultiplier)
			bumpers::CVehicleModelInfo::ms_setVehiclePhysics_origcall =
			setFnAddrInCallOpcode(g_hookAddr_CVehicleModelInfo__setPhysics, getThisCallAddr(&bumpers::CVehicleModelInfo::setVehiclePhysicsUsedMass));
		else
			bumpers::CVehicleModelInfo::ms_setVehiclePhysics_origcall =
			setFnAddrInCallOpcode(g_hookAddr_CVehicleModelInfo__setPhysics, getThisCallAddr(&bumpers::CVehicleModelInfo::setVehiclePhysics));

	}
}