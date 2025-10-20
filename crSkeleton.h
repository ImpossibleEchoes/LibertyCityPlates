#pragma once
#include "rage_math.h"

struct crBoneData {
	const char* pszName;
	uint32_t __4;
	crBoneData* m_pNextSibling;
	crBoneData* m_pFirstChild;
	crBoneData* m_pParent;
	uint16_t m_wBoneIndex;
	uint16_t m_wBoneId;
	uint16_t m_wMirror;
	uint8_t m_nbTransFlags;
	uint8_t m_nbRotFlags;
	uint8_t m_nbScaleFlags;
	Vec3 m_vOffset;
	uint32_t m_dwHash;
	Vector3 m_vRotationEuler;
	Quaternion m_vRotationQuaternion;
	Vec3 m_vScale;
	uint32_t field_5C;
	Vector3 m_vParentModelOffset;
	Vector3 m_vOrient;
	Vector3 m_vSorient;
	Vector3 m_vTransMin;
	Vector3 m_vTransMax;
	Vector3 m_vRotMin;
	Vector3 m_vRotMax;
	void* m_pJointData;
	uint32_t field_D4;
	uint32_t field_D8;
	uint32_t field_DC;
};

struct crSkeletonData {
	crBoneData* m_pBones;

	uint8_t __4[0x10];
	uint16_t m_wNumBones;

};
