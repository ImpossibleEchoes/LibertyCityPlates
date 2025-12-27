#include "addrs.h"
#include <Windows.h>
#include "vehExtParams.h"
#include "hookFns.h"
#include "trace.h"
#include "cmath"
#include "config.h"

#include "rage_math.h"
#include "CHandling.h"

#include "crSkeleton.h"
#include "utils.h"
#include <map>

#include "newComponents.h"

#include "helpers.h"
#include "CVehicle.h"

#include "fragType.h"
#include "atPool.h"




namespace pedNormalMapBlend {

struct CPed {
	
	bool getIsPlayer() const { return *((BYTE*)this + 0x219); }

	static size_t ms_prerender_origcall;
	char prerender_hoocked(CPed* pPed) {
		auto result = ((char(__thiscall*)(void*, CPed*))(ms_prerender_origcall))(this, pPed);
		
		if (pPed->getIsPlayer()) {


			auto pSkel = ((crSkeletonData * (__thiscall*)(CPed * _a))(g_CDynamicEntity__getSkeletonData))(pPed);
			auto pBone1 = pSkel->m_pBones;
			for (size_t i = 0; i < pSkel->m_wNumBones; i++) {
				if (!strcmp(pSkel->m_pBones[i].pszName, "Char_Spine2")) {
					pBone1 = pSkel->m_pBones + i;
					break;
				}
			}
			if(pBone1->m_wBoneIndex == 0)
				printf("pBone1->m_wBoneIndex == 0\n");

			auto pBone2 = pSkel->m_pBones;
			for (size_t i = 0; i < pSkel->m_wNumBones; i++) {
				if (!strcmp(pSkel->m_pBones[i].pszName, "Char_Spine1")) {
					pBone2 = pSkel->m_pBones + i;
					break;
				}
			}
			if(pBone2->m_wBoneIndex == 0)
				printf("pBone2->m_wBoneIndex == 0\n");

			auto pMtx = ((Matrix34 * (__thiscall*)(CPed * _a, int))(FIX_ADDR(0x004B6860)))(pPed, pBone1->m_wBoneIndex);
			auto pMtx2 = ((Matrix34 * (__thiscall*)(CPed * _a, int))(FIX_ADDR(0x004B6860)))(pPed, pBone2->m_wBoneIndex);

			Vector3 forward1 = pMtx->c;
			Vector3 forward2 = pMtx2->c;

			float x1 = forward1.x;
			float y1 = forward1.y;

			float x2 = forward2.x;
			float y2 = forward2.y;

			// 3. Вычисляем углы для каждого вектора
			float angle1 = atan2(y1, x1); // Угол в радианах от -PI до PI
			float angle2 = atan2(y2, x2);

			// 4. Вычисляем разницу
			float angleDiff = angle2 - angle1;

			// 5. Нормализуем разницу углов к диапазону от -PI до PI
			// (Это делает разницу "кратчайшим путем")
			while (angleDiff > RAGE_PI)
				angleDiff -= 2 * RAGE_PI;
			while (angleDiff < -RAGE_PI)
				angleDiff += 2 * RAGE_PI;


			//pMtx->identityRot();
			//pMtx->d = { 0,0,0 };

			printf("%f\n", angleDiff);

		}

		return result;
	}
};

size_t CPed::ms_prerender_origcall;

std::map<void*, int> g_nrmExt;

struct pedFX_t {
	BYTE __0[0x470];
	Vector4 vNrmValues;

	DWORD nrmValuesIndex;
	void* pShaderGroup;
	BYTE __pad[0x8];

	char regVars(int a2) {
		auto ret = ((char(__thiscall*)(pedFX_t*, int))(FIX_ADDR(0x00B8ED10)))(this, a2);

		pShaderGroup = *(void**)(a2 + 8);
		nrmValuesIndex = ((int (__thiscall*)(void * _a, const char*, char))(FIX_ADDR(0x00425700)))(*(void**)(a2 + 8), "nrmValues", false);

		if (nrmValuesIndex != 0) {
			//printf("000\n");

			g_nrmExt.insert(std::make_pair<void*, int>((void*)this, 0));
			printf("%p\n", this);
		}

		return ret;

	}

	void update(CPed* pPed) {
		((void(__thiscall*)(pedFX_t*, CPed*))(FIX_ADDR(0xB8EE60)))(this, pPed);
		//if (g_nrmExt.count(this->) > 0) {
		//	printf("_%p\n", this);
		//}
		//if (nrmValuesIndex) {

			auto pSkel = ((crSkeletonData * (__thiscall*)(CPed * _a))(g_CDynamicEntity__getSkeletonData))(pPed);
			auto pBone1 = pSkel->m_pBones;
			for (size_t i = 0; i < pSkel->m_wNumBones; i++) {
				if (!strcmp(pSkel->m_pBones[i].pszName, "Char_Spine2")) {
					pBone1 = pSkel->m_pBones + i;
					break;
				}
			}
			if (pBone1->m_wBoneIndex == 0)
				printf("pBone1->m_wBoneIndex == 0\n");

			auto pBone2 = pSkel->m_pBones;
			for (size_t i = 0; i < pSkel->m_wNumBones; i++) {
				if (!strcmp(pSkel->m_pBones[i].pszName, "Char_Spine1")) {
					pBone2 = pSkel->m_pBones + i;
					break;
				}
			}
			if (pBone2->m_wBoneIndex == 0)
				printf("pBone2->m_wBoneIndex == 0\n");

			auto pMtx = ((Matrix34 * (__thiscall*)(CPed * _a, int))(FIX_ADDR(0x004B6860)))(pPed, pBone1->m_wBoneIndex);
			auto pMtx2 = ((Matrix34 * (__thiscall*)(CPed * _a, int))(FIX_ADDR(0x004B6860)))(pPed, pBone2->m_wBoneIndex);

			Vector3 forward1 = pMtx->c;
			Vector3 forward2 = pMtx2->c;

			float x1 = forward1.x;
			float y1 = forward1.y;

			float x2 = forward2.x;
			float y2 = forward2.y;

			// 3. Вычисляем углы для каждого вектора
			float angle1 = atan2(y1, x1); // Угол в радианах от -PI до PI
			float angle2 = atan2(y2, x2);

			// 4. Вычисляем разницу
			float angleDiff = angle2 - angle1;

			// 5. Нормализуем разницу углов к диапазону от -PI до PI
			// (Это делает разницу "кратчайшим путем")
			while (angleDiff > RAGE_PI)
				angleDiff -= 2 * RAGE_PI;
			while (angleDiff < -RAGE_PI)
				angleDiff += 2 * RAGE_PI;

			//angleDiff = max(fabsf(angleDiff) * 10.f, 1.f);

			vNrmValues.x = angleDiff;

		//	printf("%f\n", angleDiff);
		//	printf("%i\n", nrmValuesIndex);
		//}
	}

	void setVars() {
		if (nrmValuesIndex > 0)
			rage__grmShaderGroup__setVarVector4((grmShaderGroup*)pShaderGroup, nrmValuesIndex, (float*) & vNrmValues);
		//printf("%i\n", nrmValuesIndex);

	}

	

	pedFX_t* clone(int _a) {
		auto fx = ((pedFX_t*(__thiscall*)(pedFX_t*, int))(FIX_ADDR(0x00B8EBA0)))(this, _a);

		fx->vNrmValues = vNrmValues;
		fx->nrmValuesIndex = nrmValuesIndex;
		fx->pShaderGroup = pShaderGroup;


//		memcpy((BYTE*)fx + 0x470, (BYTE*)this + 0x470, 0x20);

		return fx;
	}


};

void* g_ped = nullptr;

struct CPed2 {

	char sub_4B67A0() {
		g_ped = this;

		//auto pShaderGroup = g_modelPointers[*(WORD*)((BYTE*)g_ped + 0x2E)]->m_pDrawableRef->pDrawable->m_pShaderGroup;

		//a4->nrmValuesIndex = ((int(__thiscall*)(void* _a, const char*, char))(FIX_ADDR(0x00425700)))(a4->pShaderGroup, "nrmValues", false);

		auto pSkel = ((crSkeletonData * (__thiscall*)(CPed * _a))(g_CDynamicEntity__getSkeletonData))((CPed*)g_ped);
		auto pBone1 = pSkel->m_pBones;
		for (size_t i = 0; i < pSkel->m_wNumBones; i++) {
			if (!strcmp(pSkel->m_pBones[i].pszName, "Char_Spine2")) {
				pBone1 = pSkel->m_pBones + i;
				break;
			}
		}
		if (pBone1->m_wBoneIndex == 0)
			printf("pBone1->m_wBoneIndex == 0\n");

		auto pBone2 = pSkel->m_pBones;
		for (size_t i = 0; i < pSkel->m_wNumBones; i++) {
			if (!strcmp(pSkel->m_pBones[i].pszName, "Char_Spine1")) {
				pBone2 = pSkel->m_pBones + i;
				break;
			}
		}
		if (pBone2->m_wBoneIndex == 0)
			printf("pBone2->m_wBoneIndex == 0\n");

		auto pMtx = ((Matrix34 * (__thiscall*)(CPed * _a, int))(FIX_ADDR(0x004B6860)))((CPed*)g_ped, pBone1->m_wBoneIndex);
		auto pMtx2 = ((Matrix34 * (__thiscall*)(CPed * _a, int))(FIX_ADDR(0x004B6860)))((CPed*)g_ped, pBone2->m_wBoneIndex);

		Vector3 forward1 = pMtx->c;
		Vector3 forward2 = pMtx2->c;

		float x1 = forward1.x;
		float y1 = forward1.y;

		float x2 = forward2.x;
		float y2 = forward2.y;

		// 3. Вычисляем углы для каждого вектора
		float angle1 = atan2(y1, x1); // Угол в радианах от -PI до PI
		float angle2 = atan2(y2, x2);

		// 4. Вычисляем разницу
		float angleDiff = angle2 - angle1;

		// 5. Нормализуем разницу углов к диапазону от -PI до PI
		// (Это делает разницу "кратчайшим путем")
		while (angleDiff > RAGE_PI)
			angleDiff -= 2 * RAGE_PI;
		while (angleDiff < -RAGE_PI)
			angleDiff += 2 * RAGE_PI;

		angleDiff = (angleDiff) * 10.f;
		if (angleDiff > 1)
			angleDiff = 1;
		else if (angleDiff < -1)
			angleDiff = -1;

		//printf("%f\n", angleDiff);

		auto pDrawables = *(rmcDrawable***)((BYTE*)this + 0xEA8);

		//a4->vNrmValues.x = angleDiff;

		for (size_t x = 0; x < 11; x++) {
			if (pDrawables[x]) {
				auto pShaderGroup = pDrawables[x]->m_pShaderGroup;


				for (size_t i = 0; i < pShaderGroup->m_wCount; i++) {
					for (size_t j = 0; j < pShaderGroup->m_ppShaders[i]->m_instanceData.m_dwParameterCount; j++) {
						if (pShaderGroup->m_ppShaders[i]->m_instanceData.m_pParamHash[j] == 0x97E7A3FC) {
							//printf("have ext nrm\n");
							pShaderGroup->m_ppShaders[i]->m_instanceData.m_ppParameters[j]->x = angleDiff;
						}
					}
				}
			}
		}

		return ((char(__thiscall*)(CPed2*))(FIX_ADDR(0x4B67A0)))(this);
	}
};

void __cdecl sub_943090(int a1, int a2, int a3, pedFX_t* a4, int a5, int a6, int a7) {
	printDebug("vmt in ida db(base = 0x400000) 0x%08x\n", *(size_t*)a4 - g_baseAddress + 0x400000);
	((void (__cdecl*)(int a1, int a2, int a3, pedFX_t * a4, int a5, int a6, int a7))(FIX_ADDR(0x00943090)))(a1, a2, a3, a4, a5, a6, a7);

	if (g_ped) {
		a4->pShaderGroup = g_modelPointers[*(WORD*)((BYTE*)g_ped + 0x2E)]->m_pDrawableRef->pDrawable->m_pShaderGroup;
		
		a4->nrmValuesIndex = ((int(__thiscall*)(void* _a, const char*, char))(FIX_ADDR(0x00425700)))(a4->pShaderGroup, "nrmValues", false);

		auto pSkel = ((crSkeletonData * (__thiscall*)(CPed * _a))(g_CDynamicEntity__getSkeletonData))((CPed*)g_ped);
		auto pBone1 = pSkel->m_pBones;
		for (size_t i = 0; i < pSkel->m_wNumBones; i++) {
			if (!strcmp(pSkel->m_pBones[i].pszName, "Char_Spine2")) {
				pBone1 = pSkel->m_pBones + i;
				break;
			}
		}
		if (pBone1->m_wBoneIndex == 0)
			printf("pBone1->m_wBoneIndex == 0\n");

		auto pBone2 = pSkel->m_pBones;
		for (size_t i = 0; i < pSkel->m_wNumBones; i++) {
			if (!strcmp(pSkel->m_pBones[i].pszName, "Char_Spine1")) {
				pBone2 = pSkel->m_pBones + i;
				break;
			}
		}
		if (pBone2->m_wBoneIndex == 0)
			printf("pBone2->m_wBoneIndex == 0\n");

		auto pMtx = ((Matrix34 * (__thiscall*)(CPed * _a, int))(FIX_ADDR(0x004B6860)))((CPed*)g_ped, pBone1->m_wBoneIndex);
		auto pMtx2 = ((Matrix34 * (__thiscall*)(CPed * _a, int))(FIX_ADDR(0x004B6860)))((CPed*)g_ped, pBone2->m_wBoneIndex);

		Vector3 forward1 = pMtx->c;
		Vector3 forward2 = pMtx2->c;

		float x1 = forward1.x;
		float y1 = forward1.y;

		float x2 = forward2.x;
		float y2 = forward2.y;

		// 3. Вычисляем углы для каждого вектора
		float angle1 = atan2(y1, x1); // Угол в радианах от -PI до PI
		float angle2 = atan2(y2, x2);

		// 4. Вычисляем разницу
		float angleDiff = angle2 - angle1;

		// 5. Нормализуем разницу углов к диапазону от -PI до PI
		// (Это делает разницу "кратчайшим путем")
		while (angleDiff > RAGE_PI)
			angleDiff -= 2 * RAGE_PI;
		while (angleDiff < -RAGE_PI)
			angleDiff += 2 * RAGE_PI;

		//angleDiff = max(fabsf(angleDiff) * 10.f, 1.f);

		a4->vNrmValues.x = angleDiff;


		a4->setVars();
		g_ped = nullptr;
	}

}

void init() {
	//CPed::ms_prerender_origcall = setFnAddrInCallOpcode(FIX_ADDR(0x008ED878), getThisCallAddr(&CPed::prerender_hoocked));

	//size_t size = *(size_t*)FIX_ADDR(0x00CE11D8 + 1);
	//size += 0x20;
	//size = 0x470 + 0x20;
	//writeDWORD(FIX_ADDR(0x00CE11D8 + 1), size);
	//writeDWORD(FIX_ADDR(0x00B8EBA2 + 1), size);

	//writeDWORD(FIX_ADDR(0x00DA6FFC), getThisCallAddr(&pedFX_t::regVars));
	////writeDWORD(FIX_ADDR(0x00DA7004), getThisCallAddr(&pedFX_t::update));
	////writeDWORD(FIX_ADDR(0x00DA7008), getThisCallAddr(&pedFX_t::setVars));
	//writeDWORD(FIX_ADDR(0x00DA700C), getThisCallAddr(&pedFX_t::clone));

	////setFnAddrInCallOpcode(FIX_ADDR(0x007D8E3E), (size_t)sub_943090);


	//writeDWORD(FIX_ADDR(0x008EDC73 + 1), 0x5B0);
	//writeDWORD(FIX_ADDR(0x00C67495 + 1), 0x5B0);


	setFnAddrInCallOpcode(FIX_ADDR(0x008EDCA6), (size_t)getThisCallAddr(&CPed2::sub_4B67A0));
}

}



namespace reload {

struct crAnimChannel {
	char field_4;
	char m_nbType;
	__int16 field_6;

	virtual void m0() {};
	virtual void m4() {};
	virtual void m8() {};
	virtual void mC() {};
	virtual void m10() {};
	virtual int m14(float frameIndex, void* pValue) { return -1; };
	//virtual void m18() {};
	//virtual void m1C() {};
};

struct crAnimChunk {
	char m_nbTrack;
	char m_nbFormat;
	WORD m_wBoneId;
	crAnimChannel* m_channels[4];
	int m_dwChannelCount;
};

struct crAnimTrack {
	BYTE m_nbTrack;
	char m_nbType;
	WORD m_wId;
	WORD m_wFramesPerShunk;
	WORD m_wFlags;
	crAnimChunk** m_chunks;
	WORD m_numChunks;
	WORD m_memSizeChunks;
};

struct crAnimation {
	size_t __vmt;
	WORD m_wUsageCount;
	WORD m_wFlags;
	WORD m_wFrames;
	WORD m_wSequenceFrameLimit;
	float m_fDuration;
	DWORD m_dwSignature;
	//crAnimBlock m_Blocks;
	//const char* m_pszFileName;
	//rage::atArray<rage::datOwner<rage::crAnimTrack>> m_Tracks;
	//WPRD m_wExtraFlags;
	//WORD _2A;
	//int _f2C;
};

struct CAnimPlayer {
	size_t** __vmt;
	DWORD m_dwFlags;
	DWORD m_dwLayerIndex;
	DWORD _fc;
	DWORD m_nAnimAssociationId;
	DWORD m_nAnimId;
	DWORD m_dwAnimHash;
	DWORD _f1c;
	void(__cdecl* m_pfnCallback)(CAnimPlayer*, int);
	void* m_pArgForCallback;
	int m_taskPoolHandle;
	float _f2c;
	float _f30;
	float _f34;
	DWORD _f38;
	float m_fBlendOutDelta;
	crAnimation* m_pAnimation;
	WORD m_wActual; // 1 or 0
	WORD m_dwFlags2;
	DWORD _f48;
	float m_fAnimCurrentTime;
	float m_fAnimCurrentTimeOld;
	float m_fSpeed;
	float m_fBlendAmount;
	float _f5c; // old?
	float _f60;
	DWORD _f64;
	float _f68;
	DWORD _f6c;
	float _f70;
	DWORD _f74;
	float _f78;
	float _f7c;
	float _f80;
	void* _f84;
};

struct CAnimBlender_obj3 {
	int field_0;
	CAnimPlayer m_animPlayer;
	CAnimBlender_obj3* m_pNext;
	CAnimBlender_obj3* m_pPrev;
};

struct t {
	void sub_CFFC90(void*) {
		printf("sub_CFFC90(...)\n");
	}

	void sub_CFFC90_2(void*) {
		printf("sub_CFFC90_2(...)\n");
	}

	int sub_CFFC70(void*) {
		printf("sub_CFFC70(...)\n");
		return 0;
	}

	int sub_CFFC70_2(void*) {
		printf("sub_CFFC70_2(...)\n");
		return 0;
	}

	int sub_CFFC70_override(int _b) {
		int old = *(int*)((BYTE*)this + 0x54);
//		printf("%i\n", *((BYTE*)this + 0x50));
		*((BYTE*)this + 0x50) = 1;
		*(int*)((BYTE*)this + 0x58) = old;
		*(int*)((BYTE*)this + 0x54) = _b;
//		printf("state = %i,\t%i\n", old, _b);
		return old;
	}

	void setStateToStart(void* pPed) {
		//printf("to start\n");

		void* pPedWeap = (BYTE*)pPed + 0x2B0;

		auto pWeapData = ((void*(__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);

		DWORD index = *(DWORD*)((BYTE*)pWeapData + 0x18);

		auto weapInfo = ((void*(__cdecl*)(int))(FIX_ADDR(0x009AB620)))(index);
		WORD clipSize = *(WORD*)((BYTE*)weapInfo + 0x86);

		auto ammoInClip = ((int (__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x5C);
		auto numAmmo = ((int (__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x60);

		if (numAmmo != ammoInClip && ammoInClip < clipSize - 1) {


			ammoInClip++;
			//numAmmo--;

			((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))((BYTE*)pWeapData + 0x5C, ammoInClip);
			((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))((BYTE*)pWeapData + 0x60, numAmmo);


			//printf("%i\n", ammoInClip);
			//printf("%i\n", numAmmo);
			sub_CFFC70_override(0);
		}
	}

	void sub_A7A0D0(void*) {

	}

	static size_t ms_process_origcall;




	float getFirstFrameWithMask(void* pAnim, DWORD mask) {
		auto pAnimTrack = ((crAnimTrack * (__thiscall*)(void* pThis, BYTE, WORD))(FIX_ADDR(0x006FD180)))(pAnim, 0x80, 0);

		if (pAnimTrack) {
			WORD numFrames = *(WORD*)((BYTE*)pAnim + 0x8);

			crAnimation* pRealAnim = (crAnimation*)pAnim;
			//auto sequence = pRealAnim->;


			//if (numFrames == sequence) {
				DWORD value2 = 0;
				for (size_t i = 0; i < numFrames; i++) {
					pAnimTrack->m_chunks[0]->m_channels[0]->m14(i, &value2);
					if ((value2 & mask) != 0) {
						return (float)i / numFrames;
					}
				}
			//}
			//else {
			//	float seq[0x10];
			//	//for (size_t i = 0; i < numFrames / sequence + (numFrames % sequence); i++) {


			//	//}

			//}


		}
		return -1.f;
	}

	bool getFlagsValue(void* pAnim, float val, float oldVal, DWORD mask) {
		auto pAnimTrack = ((crAnimTrack * (__thiscall*)(void* pThis, BYTE, WORD))(FIX_ADDR(0x006FD180)))(pAnim, 0x80, 0);

		if (pAnimTrack) {
			WORD numFrames = *(WORD*)((BYTE*)pAnim + 0x8);

			WORD currTrack = val * numFrames;
			WORD oldTrack = oldVal * numFrames;



			DWORD value = 0;
			if (val == oldVal) {
				pAnimTrack->m_chunks[0]->m_channels[0]->m14(currTrack, &value);
			}
			else {
				DWORD value2 = 0;
				for (size_t i = oldTrack; i <= currTrack; i++) {
					pAnimTrack->m_chunks[0]->m_channels[0]->m14(i, &value2);
					if ((value2 & mask) != 0)
						return true;
					
				}
			}

			//else if (oldTrack < currTrack) {
			//	DWORD value2 = 0;
			//	bool bOn = false;
			//	for (size_t i = oldTrack; i <= currTrack; i++) {
			//		pAnimTrack->m_chunks[0]->m_channels[0]->m14(i, &value2);
			//		//value |= value2;
			//		if (bOn && (value2 & mask) != 0)
			//			return false;
			//		bOn = (value2 & mask) != 0;
			//	}
			//	return bOn;
			//}

			return (value & mask) != 0;
		}

		return false;
	}

	bool process(void* pPed) {
		auto pfnAnimPlayer__getActionFlag = ((char(__thiscall*)(void*, int a2, float* a3, float a4, float a5))(FIX_ADDR(0x00A7A430)));

		bool ret;
		{



			if (*(int*)((BYTE*)this + 0x54) == 0 && *(int*)((BYTE*)this + 0x58) == 0) {
				void* pPedWeap = (BYTE*)pPed + 0x2B0;

				auto pWeapData = ((void* (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);

				DWORD index = *(DWORD*)((BYTE*)pWeapData + 0x18);

				auto weapInfo = ((void* (__cdecl*)(int))(FIX_ADDR(0x009AB620)))(index);
				WORD clipSize = *(WORD*)((BYTE*)weapInfo + 0x86);


				auto ammoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x5C);
				auto numAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x60);

				*(DWORD*)((BYTE*)this + 0x68) = ammoInClip;
				*(DWORD*)((BYTE*)this + 0x6C) = numAmmo;

			}
			else {
				BYTE* pAnimPlayer = *(BYTE**)((BYTE*)this + 0x48);
				if (pAnimPlayer) {
					float fWhere = 0;
					if (pfnAnimPlayer__getActionFlag(pAnimPlayer, 1 << 31, &fWhere, 0.f, 1.f) && pfnAnimPlayer__getActionFlag(pAnimPlayer, 1 << 30, &fWhere, 0.f, 1.f)) {

						auto ammoInClip = *(DWORD*)((BYTE*)this + 0x68);
						auto numAmmo = *(DWORD*)((BYTE*)this + 0x6C);

						void* pPedWeap = (BYTE*)pPed + 0x2B0;

						auto pWeapData = ((void* (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);

						auto currAmmoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x5C);
						auto currNumAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x60);

						if (currAmmoInClip != ammoInClip || currNumAmmo != numAmmo) {
							((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))((BYTE*)pWeapData + 0x5C, ammoInClip);
							((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))((BYTE*)pWeapData + 0x60, numAmmo);
						}
					}
				}
			}
		}

	

		ret = ((bool(__thiscall*)(void* pThis, void* pPed))(ms_process_origcall))(this, pPed);
		//{
		//	void* pPedWeap = (BYTE*)pPed + 0x2B0;

		//	auto pWeapData = ((void* (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);

		//	DWORD index = *(DWORD*)((BYTE*)pWeapData + 0x18);

		//	auto weapInfo = ((void* (__cdecl*)(int))(FIX_ADDR(0x009AB620)))(index);
		//	WORD clipSize = *(WORD*)((BYTE*)weapInfo + 0x86);




		//	auto ammoInClip2 = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x5C);
		//	auto numAmmo2 = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x60);

		//	if (ammoInClip2 != *(DWORD*)((BYTE*)this + 0x68) || *(DWORD*)((BYTE*)this + 0x6C) != numAmmo2) {
		//		((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))((BYTE*)pWeapData + 0x5C, *(DWORD*)((BYTE*)this + 0x68));
		//		((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))((BYTE*)pWeapData + 0x60, *(DWORD*)((BYTE*)this + 0x6C));
		//	}

		//	auto ammoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x5C);
		//	auto numAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x60);

		//	*(DWORD*)((BYTE*)this + 0x68) = ammoInClip;
		//	*(DWORD*)((BYTE*)this + 0x6C) = numAmmo;


		//}

		//auto ret = ((bool(__thiscall*)(void* pThis, void* pPed))(ms_process_origcall))(this, pPed);

		BYTE* pAnimPlayer = *(BYTE**)((BYTE*)this + 0x48);

		if (pAnimPlayer) {

			float animCurrentTime = *(float*)(pAnimPlayer + 0x4C);
			float animCurrentTimeOld = *(float*)(pAnimPlayer + 0x50);

			BYTE* pAnim = *(BYTE**)(BYTE*)(pAnimPlayer + 0x40);


			float fWhere = 0;
			if (pfnAnimPlayer__getActionFlag(pAnimPlayer, 1 << 31, &fWhere, 0.f, 1.f)) {
				if (animCurrentTime >= fWhere && animCurrentTimeOld < fWhere) {

					printf("on!\n");

					float where2 = fWhere;

					bool bOk = pfnAnimPlayer__getActionFlag(pAnimPlayer, 1 << 30, &fWhere, 0.f, 1.f);

					//if (getFlagsValue(pAnim, animCurrentTime, animCurrentTimeOld, 1 << 31)) {
					//	printf("on!\n");
					//	float val = getFirstFrameWithMask(pAnim, 1 << 30);
					//	if (val > -1.f) {
					if (bOk) {
						float val = fWhere;

						//*(float*)(pAnimPlayer + 0x50) = *(float*)(pAnimPlayer + 0x4C);
						//*(float*)(pAnimPlayer + 0x4C) = 0.f;
						//sub_CFFC70_override(0);

						void* pPedWeap = (BYTE*)pPed + 0x2B0;

						auto pWeapData = ((void* (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);

						DWORD index = *(DWORD*)((BYTE*)pWeapData + 0x18);

						auto weapInfo = ((void* (__cdecl*)(int))(FIX_ADDR(0x009AB620)))(index);
						WORD clipSize = *(WORD*)((BYTE*)weapInfo + 0x86);

						auto ammoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x5C);
						auto numAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x60);


						auto pWeapObj = *(BYTE**)((BYTE*)pPedWeap + 0x2C);
						auto pAnimBlender = *(BYTE**)(pWeapObj + 0x78);


						if (numAmmo != ammoInClip && ammoInClip < clipSize) {


							ammoInClip++;
							//numAmmo--;

							((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))((BYTE*)pWeapData + 0x5C, ammoInClip);
							((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))((BYTE*)pWeapData + 0x60, numAmmo);

							{
								ammoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x5C);
								numAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))((BYTE*)pWeapData + 0x60);
								*(DWORD*)((BYTE*)this + 0x68) = ammoInClip;
								*(DWORD*)((BYTE*)this + 0x6C) = numAmmo;
							}

							//((char(__thiscall*)(void* pThis, void* pPed))(FIX_ADDR(0x00CFFD40)))(this, pPed);

							if (ammoInClip < clipSize) {

								*(DWORD*)((BYTE*)this + 0x64) = 0;
								pAnimPlayer = *(BYTE**)((BYTE*)this + 0x48);
								*(DWORD*)((BYTE*)pWeapData + 0x1C) = 2;

								//*(float*)(pAnimPlayer + 0x50) = *(float*)(pAnimPlayer + 0x4C);
								*(float*)(pAnimPlayer + 0x50) = val;
								*(float*)(pAnimPlayer + 0x4C) = val;

								int reloadIndex = 0;

								CAnimPlayer* pWeapAnimPlayer = nullptr;

								if (pAnimBlender) {
									auto pObj = *(CAnimBlender_obj3**)(pAnimBlender + 0x1A28);

									if (pObj) {
										while (true) {
											auto v4 = pObj->m_animPlayer.m_wActual == 1;
											if (!v4)
												continue;
											if (pObj->m_animPlayer.m_dwLayerIndex == reloadIndex) {
												pWeapAnimPlayer = &pObj->m_animPlayer;
												pWeapAnimPlayer->m_fAnimCurrentTime = val;
												pWeapAnimPlayer->m_fAnimCurrentTimeOld = val;
												break;
											}
											pObj = pObj->m_pNext;

											if (!pObj)
												break;
										}
									}
								}

								//printf("%i\n", ammoInClip);
								//printf("%i\n", numAmmo);
								sub_CFFC70_override(2);

							}

						}
					}
				}

			}

		}
		else {
			printf("no player\n");

		}

		return ret;
	}

	bool CPed__isDucking() {
		return false;
	}

	void* __thiscall CTaskSimpleReloadGun__CTaskSimpleReloadGun_1(int a2, float a3, float a4) {
		printf("CTaskSimpleReloadGun__CTaskSimpleReloadGun_1");
		return ((void* (__thiscall*)(void* pThis, int a2, float a3, float a4))(FIX_ADDR(0x00CFFAF0)))(this, a2, a3, a4);
	}
	void* __thiscall CTaskSimpleReloadGun__CTaskSimpleReloadGun_2(int a2, float a3, float a4) {
		printf("CTaskSimpleReloadGun__CTaskSimpleReloadGun_2");
		return ((void* (__thiscall*)(void* pThis, int a2, float a3, float a4))(FIX_ADDR(0x00CFFAF0)))(this, a2, a3, a4);
	}
	void* __thiscall CTaskSimpleReloadGun__CTaskSimpleReloadGun_3(int a2, float a3, float a4) {
		printf("CTaskSimpleReloadGun__CTaskSimpleReloadGun_3");
		return ((void* (__thiscall*)(void* pThis, int a2, float a3, float a4))(FIX_ADDR(0x00CFFAF0)))(this, a2, a3, a4);
	}
	void* __thiscall CTaskSimpleReloadGun__CTaskSimpleReloadGun_4(int a2, float a3, float a4) {
		printf("CTaskSimpleReloadGun__CTaskSimpleReloadGun_4");
		return ((void* (__thiscall*)(void* pThis, int a2, float a3, float a4))(FIX_ADDR(0x00CFFAF0)))(this, a2, a3, a4);
	}
	void* __thiscall CTaskSimpleReloadGun__CTaskSimpleReloadGun_5(int a2, float a3, float a4) {
		printf("CTaskSimpleReloadGun__CTaskSimpleReloadGun_5");
		return ((void* (__thiscall*)(void* pThis, int a2, float a3, float a4))(FIX_ADDR(0x00CFFAF0)))(this, a2, a3, a4);
	}

};

size_t t::ms_process_origcall = 0;

struct CWeaponInfo {
	int m_dwWeaponInfoId;
	int m_nSlot;
	int m_dwFireType;
	int m_dwDamageType;
	int m_dwGroup;
	float m_fTargetRange;
	float m_fWeaponRange;
	int m_statType;
	int m_dwFlags;
	int m_nWeaponModelHash;
	int m_dwAnimGroupId;
	float m_fFirerate;
	float m_fBlindfirerate;
	float m_fAccuracy;
	float m_fAccuracyfps;
	int _f3C;
	Vector4 _f40;
	Vector4 _f50;
	float m_fPitch;
	float m_fStanding;
	float m_fDucked;
	float m_fScale;
	int m_dwRumbleDuration;
	float m_fRumbleIntensity;
	int m_dwRegenTime;
	__declspec(align(8)) __int16 m_wAMMoonStreet;
	__int16 m_wBase;
	__int16 m_wFps;
	__int16 m_wClipSize;
	__int16 m_wAmmoMax;
	int m_dwTimeBetweenShots;
	float m_fForce;
	int m_dwTime;
	int m_dwFastTime;
	int m_dwCrouchTime;
	int _fA0;
	int _fA4;
	int _fA8;
	DWORD _fAC;
	float _fB0;
	float _fB4;
	float _fB8;
	__declspec(align(16)) Vector4 _fC0;
	float _fD0;
	float _fD4;
	float _fD8;
	int m_dwAnimMeleeGroup1;
	int m_dwAnimMeleeGroup2;
	int m_dwMuzzleFxHash;
	int m_dwShellFxHash;
	int m_dwTrailFxHash;
	float m_fNetworkPlayerMod;
	float m_fNetworkPedMod;
	float m_fAccuracyTime;
	int m_dwPellets;
	BYTE __100[12];
	int _f10C;
};

struct CWeaponData {
	char __0[20];
	int _f14;
	int m_dwInfoIndex;
	int _f1C;
	int _f20;
	char _f24;
	int _f28;
	int _f2C;
	int _f30;
	int _f34;
	int _f38;
	int _f3C;
	int _f40;
	int _f44;
	int _f48;
	int _f4C;
	char _f50;
	int _f54;
	char _f58;
	void* m_secbuffArray1;
	void* m_secbuffArray2;

	void* regillClip(int) {
		PRINT_DUBUG("refillClip\n");
		return this;
	}
};

struct CTaskSimpleReloadGun {
	BYTE __0[0x14];
	int _f14;
	BYTE __18[24];
	int _f30;
	int _f34;
	int _f38;
	int _f3C;
	float _f40;
	float _f44;
	CAnimPlayer* m_pPedReloadAnimPlayer;
	int _f4C;
	char _f50;
	int m_state;
	int m_stateOld;
	int _f5C;
	int _f60;
	int _f64;

	DWORD m_ammoInClip;
	DWORD m_numAmmo;

	static size_t ms_process_origcall;

	__forceinline int setState(int newState) {
		_f50 = 1;
		m_stateOld = m_state;
		m_state = newState;
		return m_stateOld;
	}

	void CPedWeaponMgr__sub_9DF420(void * pPed) {
		void* pWeapObj = *(void**)((BYTE*)this + 0x2C);
		void* pWeap2Obj = *(void**)((BYTE*)this + 0x38);

		void* pPedWeap = this;

		if (pWeap2Obj && pWeapObj) {
			auto Quaternion__fromEulersXYZ = ((void*(__thiscall*)(Vector4*, Vector3*))(FIX_ADDR(0x0044F7E0)));
			auto CEntity__attachToEntity = ((void(__thiscall*)(void*, void*, WORD, WORD, Vector3*, Vector4*))(FIX_ADDR(0x00A7E1F0)));
			auto CDynamicEntity__getSkeletonData = ((void*(__thiscall*)(void*))(FIX_ADDR(0x004B68E0)));
			auto getBoneIndexById = ((int(__cdecl*)(void*, WORD))(FIX_ADDR(0x00A801D0)));

			int id = 1232;
			if ((*(DWORD*)((BYTE*)pPed + 0x2A0) & 2) != 0)
				id = 1219;

			auto pWeapData = ((CWeaponData * (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);
			auto weapInfo = ((CWeaponInfo * (__cdecl*)(int))(FIX_ADDR(0x009AB620)))(pWeapData->m_dwInfoIndex);
			
			auto SkeletonData = CDynamicEntity__getSkeletonData(pPed);
			auto BoneIndexById = getBoneIndexById(SkeletonData, id);
			Vector4 rotQuat;
			Quaternion__fromEulersXYZ(&rotQuat, (Vector3*)&weapInfo->_fC0);
			CEntity__attachToEntity(pWeap2Obj, pPed, BoneIndexById, 258, (Vector3*)&weapInfo->_fB0, (Vector4*)&rotQuat);

		}
		else {
			((void(__thiscall*)(void* pThis, void* pPed))(FIX_ADDR(0x009DF420)))(pPedWeap, pPed);
		}

	}

	bool process(void* pPed) {
		auto pfnAnimPlayer__getActionFlag = ((char(__thiscall*)(void*, int a2, float* a3, float a4, float a5))(FIX_ADDR(0x00A7A430)));

		bool ret;
		//{

		//	if (m_state == 0 && m_stateOld == 0) {
		//		void* pPedWeap = (BYTE*)pPed + 0x2B0;

		//		auto pWeapData = ((CWeaponData * (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);

		//		auto ammoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))(&pWeapData->m_secbuffArray1);
		//		auto numAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))(&pWeapData->m_secbuffArray2);

		//		m_ammoInClip = ammoInClip;
		//		m_numAmmo = numAmmo;
		//	}
		//	else {
		//		if (m_pPedReloadAnimPlayer) {
		//			float fWhere = 0;
		//			if (pfnAnimPlayer__getActionFlag(m_pPedReloadAnimPlayer, 1 << 31, &fWhere, 0.f, 1.f) && pfnAnimPlayer__getActionFlag(m_pPedReloadAnimPlayer, 1 << 30, &fWhere, 0.f, 1.f)) {

		//				auto ammoInClip = m_ammoInClip;
		//				auto numAmmo = m_numAmmo;

		//				void* pPedWeap = (BYTE*)pPed + 0x2B0;

		//				auto pWeapData = ((CWeaponData * (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);

		//				auto currAmmoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))(&pWeapData->m_secbuffArray1);
		//				auto currNumAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))(&pWeapData->m_secbuffArray2);

		//				if (currAmmoInClip != ammoInClip || currNumAmmo != numAmmo) {
		//					((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))(&pWeapData->m_secbuffArray1, ammoInClip);
		//					((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))(&pWeapData->m_secbuffArray2, numAmmo);
		//				}
		//			}
		//		}
		//	}
		//}



		ret = ((bool(__thiscall*)(void* pThis, void* pPed))(ms_process_origcall))(this, pPed);


		if (m_pPedReloadAnimPlayer) {
			float fWhere = 0;

		//	*(DWORD*)((BYTE*)pPed + 0x2A0) |= 2;

			//if (pfnAnimPlayer__getActionFlag(m_pPedReloadAnimPlayer, 1 << 24, &fWhere, 0.f, 1.f)) {
			//	if (m_pPedReloadAnimPlayer->m_fAnimCurrentTime >= fWhere && m_pPedReloadAnimPlayer->m_fAnimCurrentTimeOld < fWhere) {
			//		//printf("wow_x2!\n");
			//		if ((*(DWORD*)((BYTE*)pPed + 0x2A0) & 2) != 0)
			//			*(DWORD*)((BYTE*)pPed + 0x2A0) &= 2;
			//		else
			//			*(DWORD*)((BYTE*)pPed + 0x2A0) |= 2;

			//		void* pPedWeap = (BYTE*)pPed + 0x2B0;
			//		auto pWeapData = ((CWeaponData* (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);
			//		auto weapInfo = ((CWeaponInfo* (__cdecl*)(int))(FIX_ADDR(0x009AB620)))(pWeapData->m_dwInfoIndex);
			//		BYTE __data[0x20];
			//		memcpy(__data, &weapInfo->_fB0, 0x20);
			//		ZeroMemory(&weapInfo->_fC0, 0x10);
			//		ZeroMemory(&weapInfo->_fB0, 0x10);
			//		weapInfo->_fC0.x = 92.406f / 57.2958f;
			//		weapInfo->_fC0.y = -10.085f / 57.2958f;
			//		weapInfo->_fC0.z = 2.636f / 57.2958f;
			//		weapInfo->_fB0 = 0.102f;
			//		weapInfo->_fB4 = 0.021f;
			//		weapInfo->_fB8 = 0.091f;

			//		auto pWeapObj = *(BYTE**)((BYTE*)pPedWeap + 0x2C);

			//		Vector4 vec1 = *(Vector4*)((BYTE*)pWeapObj + 0x10);
			//		Matrix34 mtx1 = **(Matrix34**)((BYTE*)pWeapObj + 0x20);

			//		((void(__thiscall*)(void* pThis, void* pPed))(FIX_ADDR(0x009DF420)))(pPedWeap, pPed);
			//		//((void(__thiscall*)(void* pThis, void* pPed))(FIX_ADDR(0x009DF420)))(pPedWeap, pPed);

			//		*(Vector4*)((BYTE*)pWeapObj + 0x10) = vec1;
			//		**(Matrix34**)((BYTE*)pWeapObj + 0x20) = mtx1;


			//		memcpy(&weapInfo->_fB0, __data, 0x20);

			//		if ((*(DWORD*)((BYTE*)pPed + 0x2A0) & 2) != 0)
			//			*(DWORD*)((BYTE*)pPed + 0x2A0) &= 2;
			//		else
			//			*(DWORD*)((BYTE*)pPed + 0x2A0) |= 2;


			//	}

			//}

			//float fWhere = 0;
			//if (pfnAnimPlayer__getActionFlag(m_pPedReloadAnimPlayer, 1 << 31, &fWhere, 0.f, 1.f)) {
			//	if (m_pPedReloadAnimPlayer->m_fAnimCurrentTime >= fWhere && m_pPedReloadAnimPlayer->m_fAnimCurrentTimeOld < fWhere) {

			//		printf("on!\n");

			//		float where2 = fWhere;

			//		bool bOk = pfnAnimPlayer__getActionFlag(m_pPedReloadAnimPlayer, 1 << 30, &fWhere, 0.f, 1.f);

			//		if (bOk) {
			//			float val = fWhere;

			//			void* pPedWeap = (BYTE*)pPed + 0x2B0;

			//			auto pWeapData = ((CWeaponData* (__thiscall*)(void*))(FIX_ADDR(0x009DD660)))(pPedWeap);

			//			auto weapInfo = ((CWeaponInfo* (__cdecl*)(int))(FIX_ADDR(0x009AB620)))(pWeapData->m_dwInfoIndex);
			//			WORD clipSize = weapInfo->m_wClipSize;

			//			auto ammoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))(&pWeapData->m_secbuffArray1);
			//			auto numAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))(&pWeapData->m_secbuffArray2);


			//			auto pWeapObj = *(BYTE**)((BYTE*)pPedWeap + 0x2C);
			//			auto pAnimBlender = *(BYTE**)(pWeapObj + 0x78);


			//			if (numAmmo != ammoInClip && ammoInClip < clipSize) {


			//				ammoInClip++;

			//				((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))(&pWeapData->m_secbuffArray1, ammoInClip);
			//				((int(__thiscall*)(void*, int))(FIX_ADDR(0x0048F250)))(&pWeapData->m_secbuffArray2, numAmmo);

			//				{
			//					ammoInClip = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))(&pWeapData->m_secbuffArray1);
			//					numAmmo = ((int(__thiscall*)(void*))(FIX_ADDR(0x0048F270)))(&pWeapData->m_secbuffArray2);
			//					m_ammoInClip = ammoInClip;
			//					m_numAmmo = numAmmo;
			//				}


			//				if (ammoInClip < clipSize) {

			//					_f64 = 0;
			//					pWeapData->_f1C = 2;

			//					m_pPedReloadAnimPlayer->m_fAnimCurrentTimeOld = val;
			//					m_pPedReloadAnimPlayer->m_fAnimCurrentTime = val;

			//					int reloadLayerIndex = 0;

			//					if (pAnimBlender) {
			//						auto pObj = *(CAnimBlender_obj3**)(pAnimBlender + 0x1A28);

			//						if (pObj) {
			//							while (true) {
			//								auto v4 = pObj->m_animPlayer.m_wActual == 1;
			//								if (!v4)
			//									continue;
			//								if (pObj->m_animPlayer.m_dwLayerIndex == reloadLayerIndex) {
			//									pObj->m_animPlayer.m_fAnimCurrentTime = val;
			//									pObj->m_animPlayer.m_fAnimCurrentTimeOld = val;
			//									break;
			//								}
			//								pObj = pObj->m_pNext;

			//								if (!pObj)
			//									break;
			//							}
			//						}
			//					}

			//					setState(2);

			//				}

			//			}
			//		}
			//	}

			//}

		}
		else {
			printf("no player\n");

		}

		return ret;
	}
};

size_t CTaskSimpleReloadGun::ms_process_origcall;

struct tmp_t {

};

enum ePadControls
{
	INPUT_NEXT_CAMERA,
	INPUT_SPRINT,
	INPUT_JUMP,
	INPUT_ENTER,
	INPUT_ATTACK,
	INPUT_ATTACK2,
	INPUT_AIM,
	INPUT_LOOK_BEHIND,
	INPUT_NEXT_WEAPON,
	INPUT_PREV_WEAPON,
	INPUT_NEXT_TARGET_LEFT,
	INPUT_NEXT_TARGET_RIGHT,
	INPUT_MOVE_LEFT,
	INPUT_MOVE_RIGHT,
	INPUT_MOVE_UP,
	INPUT_MOVE_DOWN,
	INPUT_LOOK_LEFT,
	INPUT_LOOK_RIGHT,
	INPUT_LOOK_UP,
	INPUT_LOOK_DOWN,
	INPUT_DUCK,
	INPUT_PHONE_TAKE_OUT,
	INPUT_PHONE_PUT_AWAY,
	INPUT_PICKUP,
	INPUT_SNIPER_ZOOM_IN,
	INPUT_SNIPER_ZOOM_OUT,
	INPUT_SNIPER_ZOOM_IN_ALTERNATE,
	INPUT_SNIPER_ZOOM_OUT_ALTERNATE,
	INPUT_COVER,
	INPUT_RELOAD,
	INPUT_VEH_MOVE_LEFT,
	INPUT_VEH_MOVE_RIGHT,
	INPUT_VEH_MOVE_UP,
	INPUT_VEH_MOVE_DOWN,
	INPUT_VEH_GUN_LEFT,
	INPUT_VEH_GUN_RIGHT,
	INPUT_VEH_GUN_UP,
	INPUT_VEH_GUN_DOWN,
	INPUT_VEH_ATTACK,
	INPUT_VEH_ATTACK2,
	INPUT_VEH_ACCELERATE,
	INPUT_VEH_BRAKE,
	INPUT_VEH_HEADLIGHT,
	INPUT_VEH_EXIT,
	INPUT_VEH_HANDBRAKE,
	INPUT_VEH_HANDBRAKE_ALT,
	INPUT_VEH_HOTWIRE_LEFT,
	INPUT_VEH_HOTWIRE_RIGHT,
	INPUT_VEH_LOOK_LEFT, // single control will break camera, pressing both will look behind
	INPUT_VEH_LOOK_RIGHT, // single control will break camera, pressing both will look behind
	INPUT_VEH_LOOK_BEHIND,
	INPUT_VEH_CIN_CAM,
	INPUT_VEH_NEXT_RADIO,
	INPUT_VEH_PREV_RADIO,
	INPUT_VEH_HORN,
	INPUT_VEH_FLY_THROTTLE_UP,
	INPUT_VEH_FLY_THROTTLE_DOWN,
	INPUT_VEH_FLY_YAW_LEFT,
	INPUT_VEH_FLY_YAW_RIGHT,
	INPUT_MELEE_ATTACK1,
	INPUT_MELEE_ATTACK2,
	INPUT_MELEE_ATTACK3,
	INPUT_MELEE_KICK,
	INPUT_MELEE_BLOCK,
	INPUT_FRONTEND_DOWN,
	INPUT_FRONTEND_UP,
	INPUT_FRONTEND_LEFT,
	INPUT_FRONTEND_RIGHT,
	INPUT_FRONTEND_RDOWN,
	INPUT_FRONTEND_RUP,
	INPUT_FRONTEND_RLEFT,
	INPUT_FRONTEND_RRIGHT,
	INPUT_FRONTEND_AXIS_X,
	INPUT_FRONTEND_AXIS_Y,
	INPUT_FRONTEND_RIGHT_AXIS_X,
	INPUT_FRONTEND_RIGHT_AXIS_Y,
	INPUT_FRONTEND_PAUSE,
	INPUT_FRONTEND_ACCEPT,
	INPUT_FRONTEND_CANCEL,
	INPUT_FRONTEND_X,
	INPUT_FRONTEND_Y,
	INPUT_FRONTEND_LB,
	INPUT_FRONTEND_RB,
	INPUT_FRONTEND_LT,
	INPUT_FRONTEND_RT,
	INPUT_MELEE_ATTACK4,
	INPUT_ZOOM_RADAR,
	INPUT_FREE_AIM,
	INPUT_MOUSE_UD,
	INPUT_MOUSE_LR,
	INPUT_MOVE_KEY_FB,
	INPUT_MOVE_KEY_LR,
	INPUT_MOVE_KEY_STUNTJUMP,
	INPUT_MOUSE_WHEEL_UP,
	INPUT_MOUSE_WHEEL_DOWN,
	INPUT_FRONTEND_AXIS_UD,
	INPUT_FRONTEND_AXIS_LR,
	INPUT_FRONTEND_LEGEND,
	INPUT_FRONTEND_APPLY,
	INPUT_FRONTEND_SHOWSCORE,
	INPUT_WEAPON_UNARMED,
	INPUT_WEAPON_MELEE,
	INPUT_WEAPON_HANDGUN,
	INPUT_WEAPON_SHOTGUN,
	INPUT_WEAPON_SMG,
	INPUT_WEAPON_RIFLE,
	INPUT_WEAPON_SNIPER,
	INPUT_WEAPON_HEAVY,
	INPUT_WEAPON_THROWN,
	INPUT_WEAPON_SPECIAL,
	INPUT_VEH_KEY_UD,
	INPUT_VEH_KEY_LR,
	INPUT_VEH_NEXT_WEAPON,
	INPUT_VEH_PREV_WEAPON,
	INPUT_TURN_OFF_RADIO,
	INPUT_FRONTEND_MINIGAME_1,
	INPUT_FRONTEND_MINIGAME_2,
	INPUT_FRONTEND_MINIGAME_3,
	INPUT_FRONTEND_MINIGAME_4,
	INPUT_FRONTEND_MODEL_SCREEN,
	INPUT_FRONTEND_VEHICLE_TOGGLE,
	INPUT_FRONTEND_DELETE_FILTER,
	INPUT_FRONTEND_REFRESH,
	INPUT_FRONTEND_PLAYER_LIST,
	INPUT_TEXT_CHAT_TOGGLE,
	INPUT_TEXT_CHAT_TEAM,
	INPUT_TEXT_CHAT_UNIVERSAL,
	INPUT_TEXT_CHAT_RESULTS_TOGGLE,
	INPUT_TEXT_CHAT_Y,
	INPUT_FRONTEND_LOCK_LOBBY,
	INPUT_KB_UP,
	INPUT_KB_DOWN,
	INPUT_KB_LEFT,
	INPUT_KB_RIGHT,
	INPUT_NEXT_TRACK,
	INPUT_PREV_TRACK,
	INPUT_DROP_WEAPON,
	INPUT_MOUSE_LMB,
	INPUT_MOUSE_RMB,
	INPUT_FE_MOUSE_UD, // helicopter mouse controls
	INPUT_FE_MOUSE_LR, // helicopter mouse controls
	INPUT_VEH_FLY_PITCH_UP,
	INPUT_VEH_FLY_PITCH_DOWN,
	INPUT_VEH_FLY_ROLL_LEFT,
	INPUT_VEH_FLY_ROLL_RIGHT,
	INPUT_REPLAY_PLAY,
	INPUT_REPLAY_SAVE_TO_HDD,
	INPUT_REPLAY_LOAD_FROM_HDD,
	INPUT_REPLAY_EXIT,
	INPUT_FRONTEND_MARKER_DELETE,
	INPUT_FRONTEND_REPLAY_PAUSE,
	INPUT_FRONTEND_REPLAY_REWIND,
	INPUT_FRONTEND_REPLAY_FFWD,
	INPUT_FRONTEND_REPLAY_NEWMARKER,
	INPUT_FRONTEND_REPLAY_RECORD,
	INPUT_FRONTEND_REPLAY_SCREENSHOT,
	INPUT_FRONTEND_REPLAY_HIDEHUD,
	INPUT_FRONTEND_REPLAY_STARTPOINT,
	INPUT_FRONTEND_REPLAY_ENDPOINT,
	INPUT_FRONTEND_REPLAY_ADVANCE,
	INPUT_FRONTEND_REPLAY_BACK,
	INPUT_FRONTEND_REPLAY_SHOWHOTKEY,
	INPUT_FRONTEND_REPLAY_TOOLS,
	INPUT_FRONTEND_REPLAY_RESTART,
	INPUT_FRONTEND_REPLAY_CYCLEMARKERLEFT,
	INPUT_FRONTEND_REPLAY_CYCLEMARKERRIGHT,
	INPUT_FRONTEND_AXIS_FWD,
	INPUT_FRONTEND_AXIS_BACK,
	INPUT_FRONTEND_AXIS_LEFT,
	INPUT_FRONTEND_AXIS_RIGHT,
	INPUT_FRONTEND_DEFINITION,
	INPUT_FRONTEND_F6,
	INPUT_FE_REPLAY_TOGGLETIME,
	INPUT_FE_REPLAY_TOGGLETIPS,
	INPUT_FE_REPLAY_PREVIEW,
	INPUT_FRONTEND_PASTE,
	INPUT_FRONTEND_CUT,
	INPUT_FRONTEND_COPY,
	INPUT_VOICE_CHAT,
	INPUT_SNIPER_ZOOM_IN_KB,
	INPUT_SNIPER_ZOOM_OUT_KB,
	INPUT_KB_PHONE_ACCEPT,
	INPUT_KB_PHONE_CANCEL,
	INPUT_VEH_ATTACK3_KB,
	INPUT_DETONATE,
	INPUT_VEH_MOVE_LEFT_2,
	INPUT_VEH_MOVE_RIGHT_2,
	NUM_INPUTS
};

struct tPadValues {
	size_t m_vfTable;//0x0
	BYTE m_nUnknown4;//0x4
	BYTE m_nContext;//0x5
	BYTE m_nCurrentValue;//0x6
	BYTE m_nLastValue;//0x7
	BYTE m_nHistoryIndex;//0x8
	BYTE pad[3];//0x9
	void* m_pHistory;//0xC
};

class CPad {
public:
	BYTE pad[0x2698];						// 0000-328C
	tPadValues m_aValues[187];					// 2698-3248
	BYTE pad2[0x41];							// 3248-3289
	BYTE m_bIsUsingKeyboardForAim;			// 3289-328A
	BYTE pad3[0x3];							// 324A-328D
	BYTE m_bIsUsingKeyboardForHeli;			// 328D-328E
	BYTE pad4[0x7DE];						// 328E-3A6C
	DWORD m_dwLastUpdateTime;					// 3A6C-3A70
	BYTE pad5[0x14];							// 3A70-3A84
};

bool isAttackPressed(void* pPed) {
	CPad* pad = ((CPad * (__thiscall*)(void* pThis))(FIX_ADDR(0x00A15320)))(pPed);
	//printf("%#08x\n", pad->m_aValues[INPUT_ATTACK].m_vfTable - (size_t)GetModuleHandleA(nullptr) - 0x400000);
	printf("%i\t%i\t%i\t%i\t%i\n", pad->m_aValues[INPUT_ATTACK].m_nUnknown4, pad->m_aValues[INPUT_ATTACK].m_nContext, pad->m_aValues[INPUT_ATTACK].m_nCurrentValue
	, pad->m_aValues[INPUT_ATTACK].m_nLastValue, pad->m_aValues[INPUT_ATTACK].m_nHistoryIndex);
	return (pad->m_aValues[INPUT_ATTACK].m_nUnknown4^ pad->m_aValues[INPUT_ATTACK].m_nCurrentValue) > 0x7F;
	//return (pad->m_aValues[INPUT_ATTACK].m_nUnknown4^ pad->m_aValues[INPUT_ATTACK].m_nCurrentValue) > 0x7F;

}

struct CTaskSimple {

	virtual ~CTaskSimple() { }

	virtual CTaskSimple* clone() { return nullptr; }
	virtual bool m8() { return true; }
	virtual int getTaskType() { return -1; } // name from unk rage game
	virtual void m10() { }
	virtual bool makeAbortable(void* pPed, BOOL bUseSmoothBlend, void* pEvent) { return false; }

	
};

struct CTaskComplex {
	BYTE __0[8];
	CTaskSimple* m_pSubTask;

	void* CTaskComplexGun__controlSubTask(void* pPed) {
		void* ret = ((void * (__thiscall*)(void* pThis, void* pPed))(FIX_ADDR(0x00A8F620)))(this, pPed);

		if (isAttackPressed(pPed)) {
			if (m_pSubTask->getTaskType() == 1043) {
			printf("attack!\n");
				((char(__thiscall*)(CTaskSimple*, void* a2, int a3, int a4))(FIX_ADDR(0x4AB560)))(m_pSubTask, pPed, 0, 0);

			}
		}
		return ret;
	}

};

struct CTaskSimpleReload : CTaskSimple {
	
	static size_t ms_process_origcall;

	bool process(void* pPed) {
		auto ret = ((bool(__thiscall*)(CTaskSimpleReload*, void* a2))(ms_process_origcall))(this, pPed);
		if (isAttackPressed(pPed)) {
			printf("attack!\n");
			//if (m_pSubTask->getTaskType() == 1043) {
				((char(__thiscall*)(CTaskSimple*, void* a2, int a3, int a4))(FIX_ADDR(0x4AB560)))(this, pPed, 1, 0);

			//}
		}

	}

	void sub_9DD8C0() {
		((void(__thiscall*)(void* a2))(FIX_ADDR(0x9DD8C0)))(this);
		printf("wow\n");
	}

	void* blendAnimationByIdAndHash(int _a, int _b, int _c, int _d, float _e) {
		printf("wow22\n");
		return ((void* (__thiscall*)(void* pThis, int _a, int _b, int _c, int _d, float _e))(FIX_ADDR(0x009D8A40)))(this, _a, _b, _c, _d, _e);

	}
};
size_t CTaskSimpleReload::ms_process_origcall;

struct CAnimAssociations__AnimData;
CAnimAssociations__AnimData* pMinigunData;

struct NameHashPair {
	int nId;
	DWORD dwHash;
};

struct AnimDescriptor {
	int animId;
	int field_4;
	int field_8;
};

struct CAnimAssociations__AnimData {
	char szAnimGroupName[24];
	int field_18;
	int field_1C;
	char szAnimFileName[24];
	int dwAnimCount;
	const char** ppszNames;
	NameHashPair* pHashNamePairs;
	AnimDescriptor* pAnimDescriptor;
	int dwAnimationId;
	int m_bIsResident;
	int field_50;
	int field_54;
};

int g_gripId;

struct tmp {
	static size_t ms_process_origcall;

	bool process(void* pPed) {


		//if (*(DWORD*)((BYTE*)this + 0x5C) == 274) {
		//	FILE* f = fopen("task_shotgun.bin", "wb");
		//	fwrite(this, 1, 0x158, f);
		//	fclose(f);

		////	*(DWORD*)((BYTE*)this + 0x5C) = 272;
		//}
		//else {
		//	FILE* f = fopen("task_rifle.bin", "wb");
		//	fwrite(this, 1, 0x158, f);
		//	fclose(f);
		//}

		//if (*(DWORD*)((BYTE*)this + 0x54) == 274 && *(DWORD*)((BYTE*)this + 0x5C) == 274) {
		//	*(DWORD*)((BYTE*)this + 0x54) = 272;
		//	*(DWORD*)((BYTE*)this + 0x5C) = 272;

		//	*(DWORD*)((BYTE*)this + 0x90) &= ~67108864;
		//	*(DWORD*)((BYTE*)this + 0xB4) = 341;
		//	*(DWORD*)((BYTE*)this + 0x130) = 0;
		//	*(DWORD*)((BYTE*)this + 0x134) = 16777216;
		//	*(DWORD*)((BYTE*)this + 0x138) = 4294967040;
		//	*(DWORD*)((BYTE*)this + 0x144) = 0;
		//	*(DWORD*)((BYTE*)this + 0x148) = 3452764160;
		//	*(DWORD*)((BYTE*)this + 0x14C) = 0;
		//	*(DWORD*)((BYTE*)this + 0x150) = 0;
		//	*(DWORD*)((BYTE*)this + 0x154) = 200;



		//}

		//printf("%i\t%i\n", *(DWORD*)((BYTE*)this + 0x54), *(DWORD*)((BYTE*)this + 0x5C));

		//*(DWORD*)((BYTE*)this + 0x90) &= ~67108864;

		printf("%i\n", *(DWORD*)((BYTE*)this + 0x90));
		return ((char(__thiscall*)(void*, void* a2))(ms_process_origcall))(this, pPed);

	}

	int processState7(void* pPed) {
		printf("state 7\n");

		return ((int(__thiscall*)(void*, void* a2))(FIX_ADDR(0x00CFDBC0)))(this, pPed);
	}

	void* createFireEvent(void* pPed, Vector4* pPos, int a4, WORD a5, float a6, float a7) {

		void* pParent = (BYTE*)pPos - 0x20;

		*(DWORD*)((BYTE*)pParent + 0x74) &= ~67108864;

		a4 &= ~67108864;

		//if (*(WORD*)((BYTE*)pParent + 0x50) != -1 && (*(DWORD*)((BYTE*)pParent + 0x50) & 0xFFFF0000) != 0xFFFF0000) {
			if(*(WORD*)((BYTE*)pParent + 0x52) == 274)
				*(WORD*)((BYTE*)pParent + 0x52) = 272;
			if(*(WORD*)((BYTE*)pParent + 0x4E) == 274)
				*(WORD*)((BYTE*)pParent + 0x4E) = 272;

		//}

		printf("changed\n");
		return ((void*(__thiscall*)(void*, void* pPed, Vector4 * pPos, int a4, WORD a5, float a6, float a7))(FIX_ADDR(0x00CFD5C0)))(this, pPed, pPos, a4, a5, a6, a7);
	}

	void attach(void*, WORD, WORD, void*, int, int) { }
	void attach2(void*, WORD, WORD, void*, void*) { }

	int setMoveBlendAnimGroup(int a4) {
		printf("new group\n");

		CAnimAssociations__AnimData* pAnims = *(CAnimAssociations__AnimData**)FIX_ADDR(0x0131F638);
		WORD count = *(WORD*)FIX_ADDR(0x0131F638 + sizeof size_t);

		FILE* f = fopen("C:\\Users\\im\\Desktop\\tmp.txt", "w");

		std::map<DWORD, const char*> __map;


		fprintf(f, "enum AnimGroupId {\n");
		for (size_t i = 0; i < count; i++) {
			fprintf(f, "\t%s = %u, // %s\n", pAnims[i].szAnimGroupName, i, pAnims[i].szAnimFileName);

			for (size_t j = 0; j < pAnims[i].dwAnimCount; j++) {
				if (!__map.count(pAnims[i].pAnimDescriptor[j].animId)) {
					__map.insert(std::make_pair(pAnims[i].pAnimDescriptor[j].animId, pAnims[i].ppszNames[j]));
				}
				else if (strcmp(__map[pAnims[i].pAnimDescriptor[j].animId], pAnims[i].ppszNames[j])) {
					__map.insert(std::make_pair(pAnims[i].pAnimDescriptor[j].animId, pAnims[i].ppszNames[j]));
				//	printf("%s\t%s\n", __map[pAnims[i].pAnimDescriptor[j].animId], pAnims[i].ppszNames[j]);
				}

			}
		}
		fprintf(f, "}\n");

		fprintf(f, "enum AnimId {\n");
		for (auto i : __map) {
			fprintf(f, "\t%s = %u,\n", i.second, i.first);

		}
		fprintf(f, "}\n");


		a4 = 59;
		return ((int(__thiscall*)(void*, int a4))(FIX_ADDR(0x00B10C00)))(this, a4);
	}

	int CPortalTracker__sub_AFCE20(char) {
		printf("CPortalTracker::sub_AFCE20\n");
		return NULL;
	}

	static size_t CPed__prerender_origcall;
	int CPed__prerender() {
		auto pPed = this;
		//if ((*(DWORD*)((BYTE*)pPed + 0x2A0) & 2) != 0)
		//	*(DWORD*)((BYTE*)pPed + 0x2A0) &= 2;
		//else
			*(DWORD*)((BYTE*)pPed + 0x2A0) |= 2;

		return ((int(__thiscall*)(void*))(CPed__prerender_origcall))(this);
	}

	void invertMatrix() {

	}

	void* invertMatrix2() {
		return this;
	}

	static Matrix34 ms_gripMatrix;
	static Matrix34* ms_pGripMatrix;

	int CWeapon__getGripIndex(void* pWeapObj, int gripId) {
		//gripId = 1;
		auto boneIndex = ((int(__thiscall*)(void*, void*, int))(FIX_ADDR(0x0099F5C0)))(this, pWeapObj, gripId);

		g_gripId = gripId;

		//if (boneIndex != -1) {
		//	ms_pGripMatrix = ((Matrix34 * (__thiscall*)(void*, int))(FIX_ADDR(0x0055FEE0)))(pWeapObj, boneIndex);
		//}
		return boneIndex;
	}

	Matrix34* Matrix34__copyFrom(Matrix34* pMtx2) {
		//Vector3 pos;
		//Vector3 rot;

		//FILE *f = fopen("data.bin", "rb");
		//if (!f)
		//	printf("!f\n");
		//else {


		//	fread(&pos, sizeof pos, 1, f);
		//	fread(&rot, sizeof pos, 1, f);
		//	fclose(f);
		//}

		//Quaternion quat2;
		//((void * (__thiscall*)(Quaternion*, Vector3*))(FIX_ADDR(0x0044F7E0)))(&quat2, &rot);
		//

		//auto pMtx = (Matrix34*)this;

		//pMtx->fromQuaternion(quat2);

		return ((Matrix34 * (__thiscall*)(void*, Matrix34*))(FIX_ADDR(0x0040AC20)))(this, ms_pGripMatrix);
		//return pMtx;
	}

	static Vector3 multiplyVector3ByMatrix3x3(const Vector3& vec, const Matrix34& mat) {
		Vector3 result;
		// mat.a, mat.b, mat.c - это столбцы матрицы
		// Для умножения V' = M * V, где M - это матрица:
		// M = | mat.a.x  mat.b.x  mat.c.x |
		//     | mat.a.y  mat.b.y  mat.c.y |
		//     | mat.a.z  mat.b.z  mat.c.z |
		result.x = vec.x * mat.a.x + vec.y * mat.b.x + vec.z * mat.c.x;
		result.y = vec.x * mat.a.y + vec.y * mat.b.y + vec.z * mat.c.y;
		result.z = vec.x * mat.a.z + vec.y * mat.b.z + vec.z * mat.c.z;
		return result;
	}

	static void multiplyMatrix34(Matrix34& result, const Matrix34& m1, const Matrix34& m2) {

		// Часть 3x3 (поворот/масштабирование): R_res = R1 * R2
		// Каждый столбец новой матрицы (result.a, result.b, result.c)
		// - это умножение соответствующего столбца m2 на матрицу R1 (m1.a, m1.b, m1.c)

		// Вычисляем result.a (первый столбец новой матрицы)
		// Это m1 * (m2.a, 0)
		result.a.x = m1.a.x * m2.a.x + m1.b.x * m2.a.y + m1.c.x * m2.a.z;
		result.a.y = m1.a.y * m2.a.x + m1.b.y * m2.a.y + m1.c.y * m2.a.z;
		result.a.z = m1.a.z * m2.a.x + m1.b.z * m2.a.y + m1.c.z * m2.a.z;

		// Вычисляем result.b (второй столбец новой матрицы)
		// Это m1 * (m2.b, 0)
		result.b.x = m1.a.x * m2.b.x + m1.b.x * m2.b.y + m1.c.x * m2.b.z;
		result.b.y = m1.a.y * m2.b.x + m1.b.y * m2.b.y + m1.c.y * m2.b.z;
		result.b.z = m1.a.z * m2.b.x + m1.b.z * m2.b.y + m1.c.z * m2.b.z;

		// Вычисляем result.c (третий столбец новой матрицы)
		// Это m1 * (m2.c, 0)
		result.c.x = m1.a.x * m2.c.x + m1.b.x * m2.c.y + m1.c.x * m2.c.z;
		result.c.y = m1.a.y * m2.c.x + m1.b.y * m2.c.y + m1.c.y * m2.c.z;
		result.c.z = m1.a.z * m2.c.x + m1.b.z * m2.c.y + m1.c.z * m2.c.z;

		// Часть трансляции: T_res = R1 * T2 + T1
		// Это умножение вектора m2.d (T2) на матрицу поворота m1 (R1)
		// и добавление вектора трансляции m1.d (T1)
		Vector3 translated_t2 = multiplyVector3ByMatrix3x3(m2.d, m1);

		result.d.x = translated_t2.x + m1.d.x;
		result.d.y = translated_t2.y + m1.d.y;
		result.d.z = translated_t2.z + m1.d.z;

	}

	static void invertMatrixNew(Matrix34* pMtx) {
		// Зберігаємо вихідні компоненти, оскільки вони будуть перезаписані
// Це критично для правильного обчислення вектора трансляції
		Vector3 original_a = pMtx->a;
		Vector3 original_b = pMtx->b;
		Vector3 original_c = pMtx->c;
		Vector3 original_d = pMtx->d; // Вектор трансляції

		// Крок 1: Транспонування верхнього 3x3 блоку (матриці обертання)
		// Нові компоненти A, B, C будуть стовпцями транспонованої матриці обертання (R_T)
		/*
		// a.x залишається без змін (original_a.x)
		// a.y стає b.x
		pMtx->a.y = original_b.x;
		// a.z стає c.x
		pMtx->a.z = original_c.x;

		// b.x стає a.y
		pMtx->b.x = original_a.y;
		// b.y залишається без змін (original_b.y)
		// b.z стає c.y
		pMtx->b.z = original_c.y;

		// c.x стає a.z
		pMtx->c.x = original_a.z;
		// c.y стає b.z
		pMtx->c.y = original_b.z;
		// c.z залишається без змін (original_c.z)
		*/
		// Крок 2: Обчислення нового вектора трансляції
		// T_inv = -R_T * T_orig
		// Де R_T - це вже транспонований 3x3 блок (a, b, c цього об'єкта)
		// T_orig - це original_d

		// Для зручності, визначимо точковий добуток (dot product)
		auto dot = [](const Vector3& v1, const Vector3& v2) {
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
			};

		// Рядки R_T (які є стовпцями вихідної R)
		Vector3 R_T_row0 = { pMtx->a.x, pMtx->a.y, pMtx->a.z }; // x-компоненти a,b,c
		Vector3 R_T_row1 = { pMtx->b.x, pMtx->b.y, pMtx->b.z }; // y-компоненти a,b,c
		Vector3 R_T_row2 = { pMtx->c.x, pMtx->c.y, pMtx->c.z }; // z-компоненти a,b,c

		// Обчислюємо T_inv = - (R_T * T_orig)
		// Зауваження: у вашому оригінальному коді були множення (z*v8) + (y*v5) + (x*v4),
		// де x,y,z були original_d.x,y,z, а v8,v5,v4 були відповідними елементами вихідної матриці.
		// Це відповідає добутку R_T * T_orig

		// Новий d.x
		pMtx->d.x = -(original_d.x * R_T_row0.x + original_d.y * R_T_row0.y + original_d.z * R_T_row0.z);
		// Зауваження: в оригінальному коді:
		// -0.0 - (float)((float)(z * v8) + (float)((float)(y * v5) + (float)(x * v4)))
		// де v4=original_a.x, v5=original_a.y, v8=original_a.z.
		// Це відповідає - (original_d.x * original_a.x + original_d.y * original_a.y + original_d.z * original_a.z)
		// що є dot product (original_d, original_a)
		// Тобто, це (- original_d dot original_a)

		// Перевіряємо відповідність оригінальному коду детальніше:
		// x = original_d.x; y = original_d.y; z = original_d.z;
		// v4 = original_a.x; v5 = original_a.y; v8 = original_a.z;
		// v9 = original_b.x; v10 = original_b.y; v6 = original_b.z;
		// v11 = original_c.x; v12 = original_c.y; v7 = original_c.z;

		// pMtx->d.x = -0.0 - (float)((float)(z * v8) + (float)((float)(y * v5) + (float)(x * v4)));
		//           = - ( (original_d.z * original_a.z) + (original_d.y * original_a.y) + (original_d.x * original_a.x) )
		//           = - (original_d dot original_a)

		// pMtx->d.y = -0.0 - (float)((float)(v6 * z) + (float)((float)(v10 * y) + (float)(v9 * x)));
		//           = - ( (original_b.z * original_d.z) + (original_b.y * original_d.y) + (original_b.x * original_d.x) )
		//           = - (original_d dot original_b)

		// pMtx->d.z = -0.0 - (float)((float)(v7 * z) + (float)((float)(v12 * y) + (float)(v11 * x)));
		//           = - ( (original_c.z * original_d.z) + (original_c.y * original_d.y) + (original_c.x * original_d.x) )
		//           = - (original_d dot original_c)

		// Отже, правильні обчислення нового вектора трансляції:
		pMtx->d.x = -(original_d.x * original_a.x + original_d.y * original_a.y + original_d.z * original_a.z);
		pMtx->d.y = -(original_d.x * original_b.x + original_d.y * original_b.y + original_d.z * original_b.z);
		pMtx->d.z = -(original_d.x * original_c.x + original_d.y * original_c.y + original_d.z * original_c.z);

	}

	Matrix34* rage__Matrix34__mul3x3(Matrix34* _a, Matrix34* _b) {
		//multiplyMatrix34(*(Matrix34*)this, *_a, *_b);
		auto pThis = (Matrix34*)this;
		*pThis = *_a;
		
		//if(g_gripId == 2)
		//	invertMatrixNew(pThis);


		((Matrix34 * (__thiscall*)(Matrix34*, Matrix34*))(FIX_ADDR(0x00448A20)))(pThis, _b);

		return _b;
	}


};
size_t tmp::ms_process_origcall;
size_t tmp::CPed__prerender_origcall;
Matrix34 tmp::ms_gripMatrix;
Matrix34* tmp::ms_pGripMatrix;



void __stdcall define(int index, const char* group, const char* subgroup, int count, char** names, void* desc, int isResident, int a8, int a9, char useFile) {

	((void (__stdcall*)(int index, const char* group, const char* subgroup, int count, char** names, void* desc, int isResident, int a8, int a9, char useFile))(FIX_ADDR(0x00928190)))
		(index, group, subgroup, count, names, desc, isResident, a8, a9, useFile);

	DWORD _index = -1;

	auto pData = ((CAnimAssociations__AnimData * (__cdecl*)(const char* group, const char* subgroup, int count, void* desc, char** names, DWORD* pIndex, int isResident, int a8, int a9, char useFile))(FIX_ADDR(0x00928270)))
		("move_newgun", "move_newgun", count, desc, names, &_index, 1, 2, 7, 1);

	//memcpy(pData, pData - 87, sizeof * pData);

	pMinigunData = pData;

	printf("%i\n", _index);
}

void init() {
	//setFnAddrInCallOpcode(FIX_ADDR(0x0099FA40), getThisCallAddr(&tmp::rage__Matrix34__mul3x3));

	//setFnAddrInCallOpcode(FIX_ADDR(0x0099F928), getThisCallAddr(&tmp::CWeapon__getGripIndex));
	//setFnAddrInCallOpcode(FIX_ADDR(0x0099FA2A), getThisCallAddr(&tmp::Matrix34__copyFrom));
	//setFnAddrInCallOpcode(FIX_ADDR(0x008C4260), getThisCallAddr(&tmp::invertMatrix2));

	//makeNop(FIX_ADDR(0x00CFFFD9), 5);
	//makeNop(FIX_ADDR(0x00CFFFE5), 4);
//	makeNop(FIX_ADDR(0x009DF4F7), 2);

//	setFnAddrInCallOpcode(FIX_ADDR(0x00CFF084), getThisCallAddr(&CTaskSimpleReload::blendAnimationByIdAndHash));

	//setFnAddrInCallOpcode(FIX_ADDR(0x009DF42E), getThisCallAddr(&CTaskSimpleReload::sub_9DD8C0));

//	CTaskSimpleReload::ms_process_origcall = writeDWORD(FIX_ADDR(0x00D8EFE4), getThisCallAddr(&CTaskComplex::CTaskComplexGun__controlSubTask));

	//tmp::ms_process_origcall = writeDWORD(FIX_ADDR(0x00DD12A8), getThisCallAddr(&tmp::process));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00CFF926), getThisCallAddr(&tmp::processState7));

//	tmp::CPed__prerender_origcall = writeDWORD(FIX_ADDR(0x00D6E550), getThisCallAddr(&tmp::CPed__prerender));

//	setFnAddrInCallOpcode(FIX_ADDR(0x009DFCCB), getThisCallAddr(&tmp::CPortalTracker__sub_AFCE20)); // setCurrentWeapon
//	setFnAddrInCallOpcode(FIX_ADDR(0x0099FA33), getThisCallAddr(&tmp::invertMatrix)); // setCurrentWeapon

	//writeBYTE(FIX_ADDR(0x00DCDE9C + 1), ' ');
	PRINT_DUBUG("%s\n", FIX_ADDR(0x00DCDE9C));
	
	writeBYTE(FIX_ADDR(0x008E9DA3 + 1), 60);
	writeBYTE(FIX_ADDR(0x008E9DD3 + 1), 60);
	//writeDWORD(FIX_ADDR(0x00928196 + 2), 144);

//	setFnAddrInCallOpcode(FIX_ADDR(0x008F1161), getThisCallAddr(&tmp::setMoveBlendAnimGroup)); // setCurrentWeapon

	//setFnAddrInCallOpcode(FIX_ADDR(0x004AB4FF), (size_t)define); // setCurrentWeapon

	//setFnAddrInCallOpcode(FIX_ADDR(0x009DFCDE), getThisCallAddr(&tmp::attach2)); // setCurrentWeapon
	//injectFunc(FIX_ADDR(0x00A7E1F0), getThisCallAddr(&tmp::attach2));

	char buf[0x20];
	char buf2[0x20];

	strcpy(buf, "move_newgun");
	strcpy(buf2, "move_newgun");

	//writeDWORD(FIX_ADDR(0x00928B25 + 1), (size_t)buf);
	//writeDWORD(FIX_ADDR(0x00928B2A + 1), (size_t)buf2);

	//setFnAddrInCallOpcode(FIX_ADDR(0x00A8EA05), getThisCallAddr(&tmp::createFireEvent));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00A8EB2C), getThisCallAddr(&tmp::createFireEvent));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00A8EAB2), getThisCallAddr(&tmp::createFireEvent));


	//float* flt = new float(0.5f);

//	setFnAddrInCallOpcode(FIX_ADDR(0x00D000AD), getThisCallAddr(&t::setStateToStart));
//	setFnAddrInCallOpcode(FIX_ADDR(0x00D00085), getThisCallAddr(&t::setStateToStart));
	
	//injectFunc(FIX_ADDR(0x0099D130), getThisCallAddr(&CWeaponData::regillClip));
//	writeDWORD(FIX_ADDR(0x00D8EFE4), getThisCallAddr(&CTaskComplex::CTaskComplexGun__controlSubTask));
	
	//makeNop(FIX_ADDR(0x009D33BF), 8); // 

//	makeNop(FIX_ADDR(0x00CFFC03), 2); // fix abort
//	setFnAddrInCallOpcode(FIX_ADDR(0x00ADB4D3), getThisCallAddr(&CWeaponData::regillClip)); // cover reload fix
	//setFnAddrInCallOpcode(FIX_ADDR(0x00ADB1EA), getThisCallAddr(&CWeaponData::regillClip));
//	setFnAddrInCallOpcode(FIX_ADDR(0x0099D303), getThisCallAddr(&CWeaponData::regillClip));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00CFFCF9), getThisCallAddr(&CWeaponData::regillClip));
	CTaskSimpleReloadGun::ms_process_origcall = writeDWORD(FIX_ADDR(0x00DD1470), getThisCallAddr(&CTaskSimpleReloadGun::process));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00CFFFF0), getThisCallAddr(&CTaskSimpleReloadGun::CPedWeaponMgr__sub_9DF420));

	//setFnAddrInCallOpcode(FIX_ADDR(0x00D00031), getThisCallAddr(&t::sub_A7A0D0));

	////setFnAddrInCallOpcode(FIX_ADDR(0x00A8EBE1), getThisCallAddr(&t::CTaskSimpleReloadGun__CTaskSimpleReloadGun_1));
	////setFnAddrInCallOpcode(FIX_ADDR(0x00A8ECFC), getThisCallAddr(&t::CTaskSimpleReloadGun__CTaskSimpleReloadGun_2));
	////setFnAddrInCallOpcode(FIX_ADDR(0x00A8EC88), getThisCallAddr(&t::CTaskSimpleReloadGun__CTaskSimpleReloadGun_3));
	////setFnAddrInCallOpcode(FIX_ADDR(0x00AD7626), getThisCallAddr(&t::CTaskSimpleReloadGun__CTaskSimpleReloadGun_4));
	////setFnAddrInCallOpcode(FIX_ADDR(0x00D000F8), getThisCallAddr(&t::CTaskSimpleReloadGun__CTaskSimpleReloadGun_5));

	//writeDWORD(FIX_ADDR(0x00CFFE8C + 3), 0);
	//makeNop(FIX_ADDR(0x00CFFE5D), 3);
	//makeNop(FIX_ADDR(0x00CFFE75), 3);

	////setFnAddrInCallOpcode(FIX_ADDR(0x00CFFFF0), getThisCallAddr(&t::sub_CFFC90));
	////setFnAddrInCallOpcode(FIX_ADDR(0x00CFFD0A), getThisCallAddr(&t::sub_CFFC90_2));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00D0007B), getThisCallAddr(&t::sub_CFFC70));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00D0006B), getThisCallAddr(&t::sub_CFFC70_2));

//	injectFunc(FIX_ADDR(0xCFFC70), getThisCallAddr(&t::sub_CFFC70_override));

	//writeDWORD(FIX_ADDR(0x00D00050 + 3), (DWORD)flt);
}

}

namespace test_ragdoll {

struct CTask {
	static atPool<CTask>** ms_pPool;

	__forceinline virtual ~CTask(){}

	__forceinline static void* operator new(size_t size) { return (*ms_pPool)->allocate(); }

	static CTask* allocateTask(){ return (*ms_pPool)->allocate(); }

};
atPool<CTask>** CTask::ms_pPool;

struct CTaskSimpleNMFallDown : CTask {

};


size_t CPlayer__getLocalPlayerPed_addr;

__declspec(naked) void* __cdecl CPlayer__getLocalPlayerPed() {
	__asm { jmp dword ptr [CPlayer__getLocalPlayerPed_addr] }
}

void process1() {
	auto pPed = CPlayer__getLocalPlayerPed();

	auto pBaseTask = CTask::allocateTask();
	
	auto result = ((CTaskSimpleNMFallDown(__thiscall*)(CTask*))(FIX_ADDR(0x00B0B580)))(pBaseTask);

}

size_t processScripts_origcall;
char processScripts() {
	auto result = ((char (*)())(processScripts_origcall))();

	return result;
}

void init() {
	CPlayer__getLocalPlayerPed_addr = FIX_ADDR(0x0086CCA0);

	processScripts_origcall = setFnAddrInCallOpcode(FIX_ADDR(0x004945B8), (size_t)processScripts);

	CTask::ms_pPool = (atPool<CTask>**)FIX_ADDR(0x015C353C);
}

}

namespace test4 {

size_t prerenderOrigcall;

struct CPed {

	bool getIsPlayer() const { return *((BYTE*)this + 0x219); }

};

struct veh {

	int prerender() {
		auto ret = ((int(__thiscall*)(veh*))(prerenderOrigcall))(this);

		CPed* driver = *(CPed**)((BYTE*)this + 0xFA0);

		if (driver && driver->getIsPlayer()) {
			void* animBlender = *(BYTE**)((BYTE*)driver + 0x78);

			auto animplayer = ((void*(__thiscall*)(void* pThis, int, int))(FIX_ADDR(0x009D48F0)))(animBlender, 72, 0x1AA);

			if (!animplayer) {
				//auto anim = ((void* (__cdecl*)(int, int))(FIX_ADDR(0x008DA450)))(49, 40);

				
				//animplayer = ((void* (__thiscall*)(void* pThis, void* pAnim, int, int, float, int, int, const char*, const char*, int, int))
				//	(FIX_ADDR(0x009D8340)))(animBlender, anim, 0, 0x0A600 | 0x20, -4.f, 49, 40, nullptr, nullptr, -1, -1);
				animplayer = ((void* (__thiscall*)(void* pThis, int, int, float, int))(FIX_ADDR(0x009D8960)))(animBlender, 72, 0x1AA, -4.f, -1);
				printf("spawn anim\n");
			}

		}

		return ret;
	}
};

void init() {
	prerenderOrigcall = setFnAddrInCallOpcode(FIX_ADDR(0x00C45785), getThisCallAddr(&veh::prerender));

}

}

namespace moreWheels {

enum eCarParts {
	PART_WHEEL_LF = 0x9,
	PART_WHEEL_RF = 0xA,
	PART_WHEEL_LM = 0xB,
	PART_WHEEL_RM = 0xC,
	PART_WHEEL_LR = 0xD,
	PART_WHEEL_RR = 0xE,

	PART_HUB_LF = 0x18,
	PART_HUB_RF = 0x19,
	PART_HUB_LR = 0x1A,
	PART_HUB_RR = 0x1B,

	PART_EXHAUST = 0x29,
	PART_EXHAUST_2 = 0x2A,
	PART_EXHAUST_3 = 0x2B,
	PART_EXHAUST_4 = 0x2C,

	PART_WHEEL_LM_1 = PART_EXHAUST,
	PART_WHEEL_RM_1 = PART_EXHAUST_2,

	PART_WHEEL_LM_2 = PART_EXHAUST_3,
	PART_WHEEL_RM_2 = PART_EXHAUST_4,


};

CVehicle* g_currVeh = nullptr;

struct CWheel_test : CWheel {

	unsigned int init(CVehicle* a2, int boneId, float fDefaultWheelRadius, float fTyreStability, int dwFlags) {
		g_currVeh = a2;
		auto ret = ((unsigned int(__thiscall*)(CWheel*, CVehicle * a2, int boneId, float fDefaultWheelRadius, float fTyreStability, int dwFlags))
			(FIX_ADDR(0x00B2A6C0)))(this, a2, boneId, fDefaultWheelRadius, fTyreStability, dwFlags);

		return ret;
	}

	unsigned int __thiscall ProcessWheelMatrixForAutomobile(void* a2, void* a3, CWheel* a4, int wheelcount) {
		return 1;
	}
};

float* __cdecl sub_9EF2C0(int a1, float* a2, float* a3, char a4) {
	printf("more wheels\n");



	

	auto ret = ((float* (__thiscall*)(CVehicleModelInfo*, float*, float*, char))(FIX_ADDR(0x00B4F6C0)))(g_modelPointers[a1], a2, a3, a4);

	CVehicle* pVeh = (CVehicle*)((BYTE*)a2 - 0x14D0);
	if (g_currVeh) {
		pVeh = g_currVeh;
		g_currVeh = nullptr;

		auto params = g_extVehParams.getFromVehicle(pVeh);
		if (!params) {
			printf("skipped null car\n");
			return ret;
		}
		auto pNewWheels = params->getMoreWheels();

		auto ppOldWheels = (CWheel**)((BYTE*)pVeh + 0xFD0);

		auto pOldWheels = *ppOldWheels;

		auto numWheels = g_pfnGetVehNumWheels(pVeh);
		memcpy(pNewWheels, *ppOldWheels, sizeof CWheel * numWheels);
		*ppOldWheels = pNewWheels;

		//memset(pOldWheels, 0, sizeof(CWheel) * numWheels);
		//for (size_t i = 0; i < numWheels; i++) {
		//	((void (__thiscall*)(CWheel*))(FIX_ADDR(0x00B2A4E0)))(pOldWheels + i);
		//}

		auto pMdlInfo = g_modelPointers[a1];
		auto pVehStruct = *(int**)((BYTE*)pMdlInfo + 0xCC);
		
		auto pSkel = ((crSkeletonData * (__thiscall*)(CVehicle * _a))(g_CDynamicEntity__getSkeletonData))(pVeh);



		FillMemory(pVehStruct + 0x29, sizeof *pVehStruct * 4, 0xFF);

		for (size_t i = 0x29; i < 0x29 + 4; i++) {
			const char* name;
			switch (i) {
			case PART_WHEEL_LM_1:
				name = "wheel_lm_1";
				break;
			case PART_WHEEL_RM_1:
				name = "wheel_rm_1";
				break;
			case PART_WHEEL_LM_2:
				name = "wheel_lm_2";
				break;
			default:
				name = "wheel_rm_2";
				break;
			}

			for (size_t j = 0; j < pSkel->m_wNumBones; j++) {
				if (!strcmp(pSkel->m_pBones[j].pszName, name)) {
					pVehStruct[i] = j;
					break;
				}
			}
		}


		if (pVehStruct[PART_WHEEL_LM_1] != -1) {
			printf("wheel_lm_1\n");

			auto pHandling = g_pfnGetHandlingData(pVeh);

			bool bSteerRWheels = (pHandling->m_dwHandlingFlags & 0x20) != 0;

			auto dwFlags = 0x1800;


			dwFlags = 0x1800;
			if (pHandling->m_fDriveRear > 0.0)
				dwFlags = 0x9800;
			if (bSteerRWheels)
				dwFlags |= 0x4000u;
			auto v20 = pHandling->m_dwModelFlags;
			if ((v20 & 0x200000) != 0)                // if bAxleRSolid then
				dwFlags |= 0x20000u;
			else if ((v20 & 0x400000) != 0)           // if bAxleRMcpherson then
				dwFlags |= 0x10000u;

			((unsigned int(__thiscall*)(CWheel*, CVehicle * a2, int boneId, float fDefaultWheelRadius, float fTyreStability, int dwFlags))
				(FIX_ADDR(0x00B2A6C0)))(pNewWheels + numWheels, pVeh, PART_WHEEL_LM_1, *(float*)((BYTE*)pMdlInfo + 0x80), pHandling->m_fSuspensionRear * 0.25f, dwFlags);


			auto num = (DWORD*)((BYTE*)pVeh + 0xFD4);
			(*num)++;
			int g = 4;
		
			numWheels = g_pfnGetVehNumWheels(pVeh);
		}
		//if (pVehStruct[PART_WHEEL_RM_1] != -1) {
		//	printf("wheel_rm_1\n");

		//	auto pHandling = g_pfnGetHandlingData(pVeh);

		//	bool bSteerRWheels = (pHandling->m_dwHandlingFlags & 0x20) != 0;

		//	auto dwFlags = 0x1800;


		//	dwFlags = 0x1000;
		//	if (pHandling->m_fDriveRear > 0.0)
		//		dwFlags = 0x9000;
		//	if (bSteerRWheels)
		//		dwFlags |= 0x4000u;
		//	auto v20 = pHandling->m_dwModelFlags;
		//	if ((v20 & 0x200000) != 0)                // if bAxleRSolid then
		//		dwFlags |= 0x20000u;
		//	else if ((v20 & 0x400000) != 0)           // if bAxleRMcpherson then
		//		dwFlags |= 0x10000u;

		//	((unsigned int(__thiscall*)(CWheel*, CVehicle * a2, int boneId, float fDefaultWheelRadius, float fTyreStability, int dwFlags))
		//		(FIX_ADDR(0x00B2A6C0)))(pNewWheels + numWheels, pVeh, PART_WHEEL_RM_1, *(float*)((BYTE*)pMdlInfo + 0x80), pHandling->m_fSuspensionRear * 0.25f, dwFlags);


		//	auto num = (DWORD*)((BYTE*)pVeh + 0xFD4);
		//	(*num)++;
		//	int g = 4;
		//
		//	numWheels = g_pfnGetVehNumWheels(pVeh);
		//}

	}
	return ret;
}

#pragma region hubComponentCrashFix

const int g_maxNumComponents = 0xff;

int* g_hubWheelId;
int* g_mirrorWheelId;
//int g_hubWheelId[g_maxNumComponents]{ -1 };
//int g_mirrorWheelId[g_maxNumComponents]{ -1 };

void initHubFix() {
	g_hubWheelId = new int[g_maxNumComponents];
	memset(g_hubWheelId, 0xFF, sizeof *g_hubWheelId * g_maxNumComponents);
	//g_hubWheelId[PART_WHEEL_LF] = PART_HUB_LF;
	//g_hubWheelId[PART_WHEEL_RF] = PART_HUB_RF;
	//g_hubWheelId[PART_WHEEL_LR] = PART_HUB_LR;
	//g_hubWheelId[PART_WHEEL_RR] = PART_HUB_RR;

	g_mirrorWheelId = new int[g_maxNumComponents];
	memset(g_mirrorWheelId, 0xFF, sizeof * g_mirrorWheelId * g_maxNumComponents);
	g_mirrorWheelId[PART_WHEEL_LF] = PART_WHEEL_RF;
	g_mirrorWheelId[PART_WHEEL_RF] = PART_WHEEL_LF;
	g_mirrorWheelId[PART_WHEEL_LR] = PART_WHEEL_RR;
	g_mirrorWheelId[PART_WHEEL_RR] = PART_WHEEL_LR;
	g_mirrorWheelId[PART_WHEEL_LM] = PART_WHEEL_RM;
	g_mirrorWheelId[PART_WHEEL_RM] = PART_WHEEL_LM;
	g_mirrorWheelId[PART_WHEEL_LM_1] = PART_WHEEL_RM_1;
	g_mirrorWheelId[PART_WHEEL_RM_1] = PART_WHEEL_LM_1;
	g_mirrorWheelId[PART_WHEEL_LM_2] = PART_WHEEL_RM_2;
	g_mirrorWheelId[PART_WHEEL_RM_2] = PART_WHEEL_LM_2;

	int* pHub = (int*)FIX_ADDR(0x00B2F8EF + 3);
	int* pMirror = (int*)FIX_ADDR(0x00B2F690 + 3);

	writeDWORD((size_t)pHub, (DWORD)g_hubWheelId);
	writeDWORD((size_t)pMirror, (DWORD)g_mirrorWheelId);
}

#pragma endregion

#pragma region unkFix
class CBaseDC
{
private:
	// Припускаємо, що vptr - це перше поле
	unsigned int vptr;
	// field_4 - друге поле
	unsigned int field_4;

	// Визначення типу для 3-го віртуального методу (індекс 2)
	// Припускаємо, що він повертає int і приймає CBaseDC* (this)
	typedef int(__thiscall* VirtualMethod)(CBaseDC*);

public:
	void execute()
	{
		// Отримання адреси віртуальної функції (VMT[2])
		//VirtualMethod virtual_func = *(VirtualMethod*)((unsigned int*)this->vptr + 2);
		//printf("execute m8: 0x%08x\n", size_t(*(void**)((unsigned int*)this->vptr + 2)) - g_baseAddress + 0x400000);

		// --- Перший виклик ---
		int v2 = 12; // Результат у EAX
		//int v2 = virtual_func(this); // Результат у EAX

		// Складний розрахунок v3. Це дуже схоже на обчислення (16 - (v2 % 16)) % 16,
		// але з бітовою логікою для обробки негативних чисел.
		// Це призводить до v3 = (16 - (v2 & 0xF)) & 0xF або подібного.
		// v2 & 0x8000000F та подальша логіка є нестандартним способом обчислення v2 & 0xF
		// для цілей вирівнювання або залишку.
		int v_a = v2 & 0x8000000F;
		if (v_a < 0) {
			v_a = ((v_a - 1) | 0xFFFFFFF0) + 1;
		}

		int v3 = 16 - v_a;
		v3 &= 0x8000000F;
		if (v3 < 0) {
			v3 = ((v3 - 1) | 0xFFFFFFF0) + 1;
		}

		// --- Другий виклик ---
		//int v4 = virtual_func(this); // Результат у EAX
		int v4 = 12; // Результат у EAX

		// --- Кінцева бітова маніпуляція ---
		int v_sum = v4 + v3;

		// Операції CDQ, ADD/SAR/SHL/XOR - це ділення v_sum на 16 (з округленням/знаком),
		// зміщення на 14 бітів, XOR з this->field_4, маскування, і XOR назад.

		// Ділення на 16 з округленням до найближчого цілого:
		int quotient = v_sum >> 4; // Sar eax, 4

		// Зсув вліво на 14:
		int intermediate = quotient << 14; // Shl eax, 0Eh

		// XOR з this->field_4:
		int v_xor = intermediate ^ this->field_4; // Xor eax, [esi+4]

		// Маскування:
		int v_final = v_xor & 0x1FFC000; // And eax, 1FFC000h

		// Зміна поля:
		this->field_4 ^= v_final; // Xor [esi+4], eax
	}
};

#pragma endregion


struct _ {

	int __(float fTimeStep, char bCanPostpone, int nTimeSlice) {
		return 0;
	}

	void __2() {

	}
};

void __cdecl sub_7EC160(CVehicle* a1) {

}

void init() {
	setFnAddrInCallOpcode(FIX_ADDR(0x00C432AC), (size_t)sub_9EF2C0);
	setFnAddrInCallOpcode(FIX_ADDR(0x00C42F32), getThisCallAddr(&CWheel_test::init));

	writeDWORD(FIX_ADDR(0x00DBA680), getThisCallAddr(&_::__));
	//setFnAddrInCallOpcode(FIX_ADDR(0x007F43FA), (size_t)sub_7EC160);
	writeDWORD(FIX_ADDR(0x00D65940), getThisCallAddr(&_::__2));

	initHubFix();

	setFnAddrInCallOpcode(FIX_ADDR(0x00B31C9B), getThisCallAddr(&CWheel_test::ProcessWheelMatrixForAutomobile));

	
	//setFnAddrInCallOpcode(FIX_ADDR(0x008A0F7E), getThisCallAddr(&CBaseDC::execute));
}

}

namespace wheelVfx {

enum ePadControls
{
	INPUT_NEXT_CAMERA,
	INPUT_SPRINT,
	INPUT_JUMP,
	INPUT_ENTER,
	INPUT_ATTACK,
	INPUT_ATTACK2,
	INPUT_AIM,
	INPUT_LOOK_BEHIND,
	INPUT_NEXT_WEAPON,
	INPUT_PREV_WEAPON,
	INPUT_NEXT_TARGET_LEFT,
	INPUT_NEXT_TARGET_RIGHT,
	INPUT_MOVE_LEFT,
	INPUT_MOVE_RIGHT,
	INPUT_MOVE_UP,
	INPUT_MOVE_DOWN,
	INPUT_LOOK_LEFT,
	INPUT_LOOK_RIGHT,
	INPUT_LOOK_UP,
	INPUT_LOOK_DOWN,
	INPUT_DUCK,
	INPUT_PHONE_TAKE_OUT,
	INPUT_PHONE_PUT_AWAY,
	INPUT_PICKUP,
	INPUT_SNIPER_ZOOM_IN,
	INPUT_SNIPER_ZOOM_OUT,
	INPUT_SNIPER_ZOOM_IN_ALTERNATE,
	INPUT_SNIPER_ZOOM_OUT_ALTERNATE,
	INPUT_COVER,
	INPUT_RELOAD,
	INPUT_VEH_MOVE_LEFT,
	INPUT_VEH_MOVE_RIGHT,
	INPUT_VEH_MOVE_UP,
	INPUT_VEH_MOVE_DOWN,
	INPUT_VEH_GUN_LEFT,
	INPUT_VEH_GUN_RIGHT,
	INPUT_VEH_GUN_UP,
	INPUT_VEH_GUN_DOWN,
	INPUT_VEH_ATTACK,
	INPUT_VEH_ATTACK2,
	INPUT_VEH_ACCELERATE,
	INPUT_VEH_BRAKE,
	INPUT_VEH_HEADLIGHT,
	INPUT_VEH_EXIT,
	INPUT_VEH_HANDBRAKE,
	INPUT_VEH_HANDBRAKE_ALT,
	INPUT_VEH_HOTWIRE_LEFT,
	INPUT_VEH_HOTWIRE_RIGHT,
	INPUT_VEH_LOOK_LEFT, // single control will break camera, pressing both will look behind
	INPUT_VEH_LOOK_RIGHT, // single control will break camera, pressing both will look behind
	INPUT_VEH_LOOK_BEHIND,
	INPUT_VEH_CIN_CAM,
	INPUT_VEH_NEXT_RADIO,
	INPUT_VEH_PREV_RADIO,
	INPUT_VEH_HORN,
	INPUT_VEH_FLY_THROTTLE_UP,
	INPUT_VEH_FLY_THROTTLE_DOWN,
	INPUT_VEH_FLY_YAW_LEFT,
	INPUT_VEH_FLY_YAW_RIGHT,
	INPUT_MELEE_ATTACK1,
	INPUT_MELEE_ATTACK2,
	INPUT_MELEE_ATTACK3,
	INPUT_MELEE_KICK,
	INPUT_MELEE_BLOCK,
	INPUT_FRONTEND_DOWN,
	INPUT_FRONTEND_UP,
	INPUT_FRONTEND_LEFT,
	INPUT_FRONTEND_RIGHT,
	INPUT_FRONTEND_RDOWN,
	INPUT_FRONTEND_RUP,
	INPUT_FRONTEND_RLEFT,
	INPUT_FRONTEND_RRIGHT,
	INPUT_FRONTEND_AXIS_X,
	INPUT_FRONTEND_AXIS_Y,
	INPUT_FRONTEND_RIGHT_AXIS_X,
	INPUT_FRONTEND_RIGHT_AXIS_Y,
	INPUT_FRONTEND_PAUSE,
	INPUT_FRONTEND_ACCEPT,
	INPUT_FRONTEND_CANCEL,
	INPUT_FRONTEND_X,
	INPUT_FRONTEND_Y,
	INPUT_FRONTEND_LB,
	INPUT_FRONTEND_RB,
	INPUT_FRONTEND_LT,
	INPUT_FRONTEND_RT,
	INPUT_MELEE_ATTACK4,
	INPUT_ZOOM_RADAR,
	INPUT_FREE_AIM,
	INPUT_MOUSE_UD,
	INPUT_MOUSE_LR,
	INPUT_MOVE_KEY_FB,
	INPUT_MOVE_KEY_LR,
	INPUT_MOVE_KEY_STUNTJUMP,
	INPUT_MOUSE_WHEEL_UP,
	INPUT_MOUSE_WHEEL_DOWN,
	INPUT_FRONTEND_AXIS_UD,
	INPUT_FRONTEND_AXIS_LR,
	INPUT_FRONTEND_LEGEND,
	INPUT_FRONTEND_APPLY,
	INPUT_FRONTEND_SHOWSCORE,
	INPUT_WEAPON_UNARMED,
	INPUT_WEAPON_MELEE,
	INPUT_WEAPON_HANDGUN,
	INPUT_WEAPON_SHOTGUN,
	INPUT_WEAPON_SMG,
	INPUT_WEAPON_RIFLE,
	INPUT_WEAPON_SNIPER,
	INPUT_WEAPON_HEAVY,
	INPUT_WEAPON_THROWN,
	INPUT_WEAPON_SPECIAL,
	INPUT_VEH_KEY_UD,
	INPUT_VEH_KEY_LR,
	INPUT_VEH_NEXT_WEAPON,
	INPUT_VEH_PREV_WEAPON,
	INPUT_TURN_OFF_RADIO,
	INPUT_FRONTEND_MINIGAME_1,
	INPUT_FRONTEND_MINIGAME_2,
	INPUT_FRONTEND_MINIGAME_3,
	INPUT_FRONTEND_MINIGAME_4,
	INPUT_FRONTEND_MODEL_SCREEN,
	INPUT_FRONTEND_VEHICLE_TOGGLE,
	INPUT_FRONTEND_DELETE_FILTER,
	INPUT_FRONTEND_REFRESH,
	INPUT_FRONTEND_PLAYER_LIST,
	INPUT_TEXT_CHAT_TOGGLE,
	INPUT_TEXT_CHAT_TEAM,
	INPUT_TEXT_CHAT_UNIVERSAL,
	INPUT_TEXT_CHAT_RESULTS_TOGGLE,
	INPUT_TEXT_CHAT_Y,
	INPUT_FRONTEND_LOCK_LOBBY,
	INPUT_KB_UP,
	INPUT_KB_DOWN,
	INPUT_KB_LEFT,
	INPUT_KB_RIGHT,
	INPUT_NEXT_TRACK,
	INPUT_PREV_TRACK,
	INPUT_DROP_WEAPON,
	INPUT_MOUSE_LMB,
	INPUT_MOUSE_RMB,
	INPUT_FE_MOUSE_UD, // helicopter mouse controls
	INPUT_FE_MOUSE_LR, // helicopter mouse controls
	INPUT_VEH_FLY_PITCH_UP,
	INPUT_VEH_FLY_PITCH_DOWN,
	INPUT_VEH_FLY_ROLL_LEFT,
	INPUT_VEH_FLY_ROLL_RIGHT,
	INPUT_REPLAY_PLAY,
	INPUT_REPLAY_SAVE_TO_HDD,
	INPUT_REPLAY_LOAD_FROM_HDD,
	INPUT_REPLAY_EXIT,
	INPUT_FRONTEND_MARKER_DELETE,
	INPUT_FRONTEND_REPLAY_PAUSE,
	INPUT_FRONTEND_REPLAY_REWIND,
	INPUT_FRONTEND_REPLAY_FFWD,
	INPUT_FRONTEND_REPLAY_NEWMARKER,
	INPUT_FRONTEND_REPLAY_RECORD,
	INPUT_FRONTEND_REPLAY_SCREENSHOT,
	INPUT_FRONTEND_REPLAY_HIDEHUD,
	INPUT_FRONTEND_REPLAY_STARTPOINT,
	INPUT_FRONTEND_REPLAY_ENDPOINT,
	INPUT_FRONTEND_REPLAY_ADVANCE,
	INPUT_FRONTEND_REPLAY_BACK,
	INPUT_FRONTEND_REPLAY_SHOWHOTKEY,
	INPUT_FRONTEND_REPLAY_TOOLS,
	INPUT_FRONTEND_REPLAY_RESTART,
	INPUT_FRONTEND_REPLAY_CYCLEMARKERLEFT,
	INPUT_FRONTEND_REPLAY_CYCLEMARKERRIGHT,
	INPUT_FRONTEND_AXIS_FWD,
	INPUT_FRONTEND_AXIS_BACK,
	INPUT_FRONTEND_AXIS_LEFT,
	INPUT_FRONTEND_AXIS_RIGHT,
	INPUT_FRONTEND_DEFINITION,
	INPUT_FRONTEND_F6,
	INPUT_FE_REPLAY_TOGGLETIME,
	INPUT_FE_REPLAY_TOGGLETIPS,
	INPUT_FE_REPLAY_PREVIEW,
	INPUT_FRONTEND_PASTE,
	INPUT_FRONTEND_CUT,
	INPUT_FRONTEND_COPY,
	INPUT_VOICE_CHAT,
	INPUT_SNIPER_ZOOM_IN_KB,
	INPUT_SNIPER_ZOOM_OUT_KB,
	INPUT_KB_PHONE_ACCEPT,
	INPUT_KB_PHONE_CANCEL,
	INPUT_VEH_ATTACK3_KB,
	INPUT_DETONATE,
	INPUT_VEH_MOVE_LEFT_2,
	INPUT_VEH_MOVE_RIGHT_2,
	NUM_INPUTS
};

struct tPadValues {
	size_t m_vfTable;//0x0
	BYTE m_nUnknown4;//0x4
	BYTE m_nContext;//0x5
	BYTE m_nCurrentValue;//0x6
	BYTE m_nLastValue;//0x7
	BYTE m_nHistoryIndex;//0x8
	BYTE pad[3];//0x9
	void* m_pHistory;//0xC
};

class CPad {
public:
	BYTE pad[0x2698];						// 0000-328C
	tPadValues m_aValues[187];					// 2698-3248
	BYTE pad2[0x41];							// 3248-3289
	BYTE m_bIsUsingKeyboardForAim;			// 3289-328A
	BYTE pad3[0x3];							// 324A-328D
	BYTE m_bIsUsingKeyboardForHeli;			// 328D-328E
	BYTE pad4[0x7DE];						// 328E-3A6C
	DWORD m_dwLastUpdateTime;					// 3A6C-3A70
	BYTE pad5[0x14];							// 3A70-3A84
};

bool isVehAttackPressed(void* pPed) {
	CPad* pad = ((CPad * (__thiscall*)(void* pThis))(g_CPed__getPad2))(pPed);
	if (pad)
		return (pad->m_aValues[INPUT_VEH_ATTACK].m_nUnknown4 ^ pad->m_aValues[INPUT_VEH_ATTACK].m_nCurrentValue) > 0x7F;
	else
		return false;
}

CVehicle* g_pVeh = nullptr;


struct fragType {
	BYTE __0[0xD0];
	fragTypeGroup** m_ppGroups;
};

struct new_CVehicleModelInfo {
	BYTE __0[0x8];
	fragType* m_pFragType;
	BYTE __C[0x60];
	DWORD m_dwVehicleType;
	BYTE __70[0x1C];
	DWORD m_dwHandlingIndex;
	BYTE __90[0x3C];

};

struct phJoint {
	int __vmt;
	BYTE gap4[12];
	int type;
	float field_14;
	int field_18;
	int field_1C;
	char field_20[144];
	char field_B0[144];
	Vec3 field_140;
	int field_14C;
	Vec3 field_150;
	int field_15C;
	Vec3 field_160;
	int field_16C;
	Vec3 field_170;
	int field_17C;
	Vec3 field_180;
	int field_18C;
	Vec3 field_190;
	int field_19C;
	Vec3 field_1A0;
	int field_1AC;
	Vec3 field_1B0;
	int field_1BC;
	float field_1C0;
	__declspec(align(8)) float field_1C8;
	__declspec(align(8)) float field_1D0;
	__declspec(align(8)) float field_1D8;
	__declspec(align(8)) float field_1E0;
	__declspec(align(8)) float field_1E8;
	BYTE gap1EC[20];
	float field_200;
	float field_204;
	float field_208;
	int field_20C;
	float field_210;
	__declspec(align(8)) float field_218;
	__declspec(align(8)) int field_220;
	char field_224;
	char field_225[3];
};

struct phJoint1Dof : phJoint {
	__declspec(align(16)) float field_230;
	__declspec(align(8)) float field_238;
	__declspec(align(8)) float field_240;
	__declspec(align(8)) float field_248;
	__declspec(align(8)) float field_250;
	__declspec(align(8)) float field_258;
	BYTE gap25C[20];
	float field_270;
	float field_274;
	float field_278;
	float field_27C;
	float m_fCurAngle;
	int field_284;
	int field_288;
	__declspec(align(8)) Vec3 field_290;
	__declspec(align(8)) float field_2A0;
	float field_2A4;
	float field_2A8;
	__declspec(align(8)) float m_fMuscleAngleStrength;
	float m_fMuscleSpeedStrength;
	float field_2B8;
	float field_2BC;
	float field_2C0;
	float field_2C4;
	__declspec(align(16)) float field_2D0;
	float field_2D4;
	float field_2D8;
	__declspec(align(8)) float field_2E0;
	float field_2E4;
	float field_2E8;
	__declspec(align(8)) float field_2F0;
	float field_2F4;
	float field_2F8;
	__declspec(align(8)) float field_300;
	float field_304;
	float field_308;
	BYTE gap30C[8];
	float field_314;
	float field_318;
	int field_31C;
};

struct fragCacheEntry {
	BYTE __0[0x1B0];
	phJoint1Dof** m_ppJoints;

};

struct fragInst {
	BYTE __0[0x64];
	fragCacheEntry* pCache;
};

struct cc : CVehicle {

	static size_t CDmgManager__getEngineHealth_origcall;

	float CDmgManager__getEngineHealth() {
		g_pVeh = *(CVehicle**)this;
		return ((float(__thiscall*)(cc*))(CDmgManager__getEngineHealth_origcall))(this);
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

	void processCannon() {
		//{
		//	auto pSkel = ((crSkeletonData * (__thiscall*)(CVehicle * _a))(g_CDynamicEntity__getSkeletonData))(this);

		//	for (size_t j = 0; j < pSkel->m_wNumBones; j++) {
		//		if (!strcmp(pSkel->m_pBones[j].pszName, "test_bone")) {

		//			auto pModelInfo = (new_CVehicleModelInfo*)g_modelPointers[getModelIndex()];
		//			

		//			int groupIndex = ((int(__cdecl*)(fragType*, int))(g_fragType__getGroupIndexByBoneIndex))(pModelInfo->m_pFragType, j);

		//			if (groupIndex != -1) {
		//				//auto pGroup = m_pFragType->m_ppGroups[groupIndex];

		//				auto pCacheEntry = *(fragCacheEntry**)((BYTE*)this + 0x100);

		//				auto pFragObj = ((fragInst * (__thiscall*)(CVehicle*))(*(void***)this)[0xA0 / sizeof size_t])(this);
		//				if (pFragObj) {
		//					//pCacheEntry = ((fragCacheEntry * (__thiscall*)(void*))(*(void***)pFragObj)[0xE0 / sizeof size_t])(pFragObj);
		//					//if (pCacheEntry) {
		//						pCacheEntry = pFragObj->pCache;

		//					//}
		//				}
		//				else
		//					pCacheEntry = *(fragCacheEntry**)((BYTE*)this + 0x100);


		//				if (pCacheEntry) {
		//					//printf("vmt in ida db(base = 0x400000) 0x%08x\n",*(size_t*)((BYTE*)pCacheEntry + 0x18 + 8) /*- g_baseAddress + 0x400000*/);

		//					
		//					
		//					auto pUnk = *(int**)((BYTE*)pCacheEntry + 0x174);

		//					int v5 = 1 - 1;
		//					auto pUnk2 = pUnk[v5 + 33];

		//					auto pJoint = (phJoint1Dof*)pUnk2;

		//					
		//					//pJoint->field_280[0] = 0.1f;

		//					pCacheEntry->m_ppJoints[groupIndex];

		//					auto ptr = &pSkel->m_pBones[j].m_vRotMin;
		//					//auto ptr = pCacheEntry->m_ppJoints[groupIndex];

		//					auto pDriver = *(BYTE**)((BYTE*)this + 0xFA0);
		//					if (pDriver) {
		//						Vector3 tmpVec;
		//						getVelocity(&tmpVec);

		//						if (((bool(__thiscall*)(void*))(FIX_ADDR(0x004C3020)))(pDriver)) {
		//							if (isVehAttackPressed(pDriver)) {
		//								//printf("0x%08x\t%p\n", ptr, ptr);
		//								//printf("%f\t%p\n", pJoint->m_fCurAngle, &pJoint->m_fCurAngle);
		//								////auto pFragObj = ((fragInst * (__thiscall*)(phJoint1Dof*, float, float))(*(void***)pJoint)[0x3C / sizeof size_t])(pJoint, -0.1f, 0.1f);
		//								//printf("%f\t%p\n", pJoint->m_fCurAngle, &pJoint->m_fCurAngle);

		//								pJoint->field_2B8 = 0.5f;
		//								//pJoint->field_2BC = 0.5f;
		//								pJoint->m_fMuscleSpeedStrength = 10.0f;
		//								pJoint->field_220 = 3;
		//								pJoint->field_2C0 =  500.f;
		//								pJoint->field_2C4 = -500.f;
		//								pJoint->m_fMuscleAngleStrength = 50.0f;
		//							}

		//							if (((bool(__cdecl*)(int))(FIX_ADDR(0x00C187F0)))(10)) {
		//								FILE* f = fopen("joint.bin", "wb");
		//								fwrite(pJoint, sizeof * pJoint, 1, f);
		//								fclose(f);
		//							}
		//							if (((bool(__cdecl*)(int))(FIX_ADDR(0x00C187F0)))(11)) {
		//								FILE* f = fopen("joint.bin", "rb");
		//								fread(pJoint, sizeof * pJoint, 1, f);
		//								fclose(f);
		//							}


		//						}
		//					}

		//					auto dword_F21B5C = (DWORD**)FIX_ADDR(0x00F21B5C);

		//					float v7 = 0;
		//					float v37 = 0;
		//					Matrix34* v10 = nullptr;

		//					auto v8 = dword_F21B5C[3];
		//					if (v8 != nullptr) {
		//						auto v9 = ((BYTE * (__thiscall*)(void*, int a2, int a3))(FIX_ADDR(0x9E24A0)))(v8, 2, 0);

		//						if (v9) {

		//							if (v9[556] == 1)
		//								v7 = 0.087266468;
		//							else {
		//								if ((unsigned int)(v9[556] - 2) > 1)
		//									goto LABEL_17_;
		//								v7 = 0.043633234;
		//							}
		//							v37 = v7;
		//						LABEL_17_:
		//							v10 = (Matrix34*)(v9 + 16);

		//						}
		//					}

		//					if (!v10) {
		//						v10 = (Matrix34*)FIX_ADDR(0x110DEC0);

		//					}

		//					Matrix34 v43 = *v10;

		//					Vector3 vec;

		//					((Vector3 * (__thiscall*)(Matrix34*, Vector3*))(FIX_ADDR(0x0090AA10)))(&v43, &vec);

		//					if (fabsf(vec.x) > 1.2217305) {
		//						float v11 = sub_627940(vec.x);
		//						auto a1a = v11 * 1.2217305 - vec.x;
		//						((void(__thiscall*)(Matrix34*, float))(FIX_ADDR(0x005D2340)))(&v43, a1a);
		//					}

		//					Vector3 v40{ 0.f, 1000.f, 0.f };

		//					const float old_x = v40.x;
		//					const float old_y = v40.y;
		//					const float old_z = v40.z;

		//					const float new_x = (v43.a.x * old_x) + (v43.b.x * old_y) + (v43.c.x * old_z) + v43.d.x;
		//					const float new_y = (v43.a.y * old_x) + (v43.b.y * old_y) + (v43.c.y * old_z) + v43.d.y;
		//					const float new_z = (v43.a.z * old_x) + (v43.b.z * old_y) + (v43.c.z * old_z) + v43.d.z;

		//					v40.x = new_x;
		//					v40.y = new_y;
		//					v40.z = new_z;

		//					float* pWaterCannonOrientation = (float*)((BYTE*)this + 0x14E8);
		//					float* pWaterCannonElevation = (float*)((BYTE*)this + 0x14EC);

		//					float fWaterCannonOrientationOld = *pWaterCannonOrientation;
		//					float fWaterCannonElevationOld = *pWaterCannonOrientation;


		//					auto pCoord = getTransform();
		//					Vector3 vec2{ v40.x - pCoord->d.x, v40.y - pCoord->d.y, v40.z - pCoord->d.z };
		//					Vector3 vec3{ (float)((float)(pCoord->a.y * vec2.y) + (float)(vec2.x * pCoord->a.x)) + (float)(pCoord->a.z * vec2.z),
		//						(float)((float)(pCoord->b.y * vec2.y) + (float)(vec2.x * pCoord->b.x)) + (float)(pCoord->b.z * vec2.z),
		//						(float)((float)(pCoord->c.y * vec2.y) + (float)(pCoord->c.x * vec2.x)) + (float)(pCoord->c.z * vec2.z) };

		//					((void(__thiscall*)(CVehicle*, Vector3*, float))(FIX_ADDR(0xA927D0)))(this, &vec3, 2.f);
		//					((void(__thiscall*)(CVehicle*, Vector3*, float, float))(FIX_ADDR(0xA91CA0)))(this, &vec3, 1.f, v37);


		//					float v21 = *pWaterCannonElevation;
		//					float v22 = -0.15000001;
		//					if (v21 >= -0.15000001)
		//					{
		//						v22 = 0.78539819;
		//						if (v21 <= 0.78539819)
		//							v22 = *pWaterCannonElevation;
		//					}
		//					*pWaterCannonElevation = v22;


		//					if (pDriver) {
		//						if (((bool(__thiscall*)(void*))(FIX_ADDR(0x004C3020)))(pDriver)) {
		//							float min = normalizeAngle(*pWaterCannonOrientation - 0.01f);
		//							float max = normalizeAngle(*pWaterCannonOrientation + 0.01f);

		//							pJoint->field_2B8 = *pWaterCannonOrientation;
		//							//pJoint->field_2BC = *pWaterCannonOrientation;
		//							pJoint->field_2BC = 0.f; // без этого будет сдиг угла на 

		//							printf("%f\t%f\t%f\t%f\t%f\n", pJoint->m_fCurAngle, *pWaterCannonOrientation, pJoint->field_270, min, max);

		//							//auto pFragObj = ((fragInst * (__thiscall*)(phJoint1Dof*, float, float))(*(void***)pJoint)[0x3C / sizeof size_t])(pJoint, min, max);


		//						}
		//					}
		//				}
		//			}
		//		}
		//	}
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

					if(pCamera[556] == 1)
						xAngleOffset = 0.087266468;
					else if(pCamera[556] == 2 || pCamera[556] == 3)
						xAngleOffset = 0.043633234;

					pCamMatrix = (Matrix34*)(pCamera + 16);

				//	if (pCamera[556] == 1)
				//		v7 = 0.087266468;
				//	else {
				//		if ((unsigned int)(pCamera[556] - 2) > 1)
				//			goto LABEL_17;
				//		v7 = 0.043633234;
				//	}
				//	v37 = v7;
				//LABEL_17:
				//	v10 = (Matrix34*)(v9 + 16);

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

				weapons->m_fTurret1Orientation = calculateTurretOrientation(&vec3, weapons->m_fTurret1Orientation, 2.f);

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

				weapons->m_fTurret1Elevation = calculateTurretElevation(&vec3, weapons->m_fTurret1Elevation, 2.f, xAngleOffset);

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

				weapons->m_fTurret2Orientation = calculateTurretOrientation(&vec3, weapons->m_fTurret2Orientation, 2.f);

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

				weapons->m_fTurret2Elevation = calculateTurretElevation(&vec3, weapons->m_fTurret2Elevation, 2.f, xAngleOffset);

				pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_2BARREL]);
				pMtx->identityRot();
				pMtx->b.y = cosf(-weapons->m_fTurret2Elevation);
				pMtx->b.z = -sinf(-weapons->m_fTurret2Elevation);
				pMtx->c.y = sinf(-weapons->m_fTurret2Elevation);
				pMtx->c.z = cosf(-weapons->m_fTurret2Elevation);
			}

		//	auto pCoord = getTransform();
		//	Vector3 vec2{ v40.x - pCoord->d.x, v40.y - pCoord->d.y, v40.z - pCoord->d.z };
		//	Vector3 vec3{ (float)((float)(pCoord->a.y * vec2.y) + (float)(vec2.x * pCoord->a.x)) + (float)(pCoord->a.z * vec2.z),
		//		(float)((float)(pCoord->b.y * vec2.y) + (float)(vec2.x * pCoord->b.x)) + (float)(pCoord->b.z * vec2.z),
		//		(float)((float)(pCoord->c.y * vec2.y) + (float)(pCoord->c.x * vec2.x)) + (float)(pCoord->c.z * vec2.z) };

		//	if (pStruct->m_aBones[COMPONENT_TURRET_1BASE] != -1 || pStruct->m_aBones[COMPONENT_TURRET_2BASE] != -1)
		//		//((void(__thiscall*)(CVehicle*, Vector3*, float))(FIX_ADDR(0xA927D0)))(this, &vec3, 2.f); // Z
		//		*pWaterCannonOrientation = calculateTurretOrientation(&vec3, *pWaterCannonOrientation, 2.f);
		//	if (pStruct->m_aBones[COMPONENT_TURRET_1BARREL] != -1 || pStruct->m_aBones[COMPONENT_TURRET_2BARREL] != -1) {
		//		//((void(__thiscall*)(CVehicle*, Vector3*, float, float))(FIX_ADDR(0xA91CA0)))(this, &vec3, 1.f, v37); // X
		//		*pWaterCannonElevation = calculateTurretElevation(&vec3, *pWaterCannonElevation, 1.f, v37);

		//		float v21 = *pWaterCannonElevation;
		//		float v22 = -0.15000001;
		//		if (v21 >= -0.15000001) {
		//			v22 = 0.78539819;
		//			if (v21 <= 0.78539819)
		//				v22 = *pWaterCannonElevation;
		//		}
		//		*pWaterCannonElevation = v22;

		//		if (pStruct->m_aBones[COMPONENT_TURRET_1BASE] != -1) {
		//			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_1BASE]);
		//			pMtx->identityRot();
		//			pMtx->a.x = cosf(-*pWaterCannonOrientation);
		//			pMtx->a.y = -sinf(-*pWaterCannonOrientation);
		//			pMtx->b.x = sinf(-*pWaterCannonOrientation);
		//			pMtx->b.y = cosf(-*pWaterCannonOrientation);
		//		}
		//		if (pStruct->m_aBones[COMPONENT_TURRET_2BASE] != -1) {
		//			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_2BASE]);
		//			pMtx->identityRot();
		//			pMtx->a.x = cosf(-*pWaterCannonOrientation);
		//			pMtx->a.y = -sinf(-*pWaterCannonOrientation);
		//			pMtx->b.x = sinf(-*pWaterCannonOrientation);
		//			pMtx->b.y = cosf(-*pWaterCannonOrientation);
		//		}

		//		if (pStruct->m_aBones[COMPONENT_TURRET_1BARREL] != -1) {
		//			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_1BARREL]);
		//			pMtx->identityRot();
		//			pMtx->b.y = cosf(-*pWaterCannonElevation);
		//			pMtx->b.z = -sinf(-*pWaterCannonElevation);
		//			pMtx->c.y = sinf(-*pWaterCannonElevation);
		//			pMtx->c.z = cosf(-*pWaterCannonElevation);
		//		}
		//		if (pStruct->m_aBones[COMPONENT_TURRET_2BARREL] != -1) {
		//			auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pStruct->m_aBones[COMPONENT_TURRET_2BARREL]);
		//			pMtx->identityRot();
		//			pMtx->b.y = cosf(-*pWaterCannonElevation);
		//			pMtx->b.z = -sinf(-*pWaterCannonElevation);
		//			pMtx->c.y = sinf(-*pWaterCannonElevation);
		//			pMtx->c.z = cosf(-*pWaterCannonElevation);
		//		}

		//	}

		}

		auto pDriver = *(BYTE**)((BYTE*)this + 0xFA0);
		if (pDriver) {
			if (((bool(__thiscall*)(void*))(g_CPed__getIsPlayer))(pDriver)) {
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

		}



		//if (pSkel) {
		//	for (size_t j = 0; j < pSkel->m_wNumBones; j++) {
		//		if (!strcmp(pSkel->m_pBones[j].pszName, "weapon_a")) {
		//			auto pWeapon = *(BYTE**)((BYTE*)this + 0x14E0);

		//			if (pWeapon) {
		//				((void(__thiscall*)(void*, CVehicle*))(FIX_ADDR(0x0099D190)))(pWeapon, this); // update weapon
		//			}
		//			else {
		//				*(void**)((BYTE*)this + 0x14E0) = ((void*(__cdecl*)(size_t))(FIX_ADDR(0x004011D0)))(0x70); // new
		//				pWeapon = *(BYTE**)((BYTE*)this + 0x14E0);

		//				if (pWeapon)
		//					((void(__thiscall*)(void*))(FIX_ADDR(0x0099D990)))(pWeapon); // CWeapon::CWeapon

		//				if (pWeapon) {
		//					printf("gun created\n");
		//					((int(__thiscall*)(void*, int a2, unsigned __int16 a3, char a4))(FIX_ADDR(0x0099D9D0)))(pWeapon, 20, 25000, 0); // CWeapon::initialize
		//				}


		//			}

		//			{

		//				//auto dword_F21B5C = (DWORD**)FIX_ADDR(0x00F21B5C);

		//				//float v7 = 0;
		//				//float v37 = 0;
		//				//Matrix34* v10 = nullptr;

		//				//auto v8 = dword_F21B5C[3];
		//				//if (v8 != nullptr) {
		//				//	auto v9 = ((BYTE * (__thiscall*)(void*, int a2, int a3))(FIX_ADDR(0x9E24A0)))(v8, 2, 0);

		//				//	if (v9) {

		//				//		if (v9[556] == 1)
		//				//			v7 = 0.087266468;
		//				//		else {
		//				//			if ((unsigned int)(v9[556] - 2) > 1)
		//				//				goto LABEL_17;
		//				//			v7 = 0.043633234;
		//				//		}
		//				//		v37 = v7;
		//				//	LABEL_17:
		//				//		v10 = (Matrix34*)(v9 + 16);

		//				//	}
		//				//}

		//				//if (!v10) {
		//				//	v10 = (Matrix34*)FIX_ADDR(0x110DEC0);

		//				//}

		//				//Matrix34 v43 = *v10;

		//				//Vector3 vec;

		//				//((Vector3 * (__thiscall*)(Matrix34*, Vector3*))(FIX_ADDR(0x0090AA10)))(&v43, &vec);

		//				//if (fabsf(vec.x) > 1.2217305) {
		//				//	float v11 = sub_627940(vec.x);
		//				//	auto a1a = v11 * 1.2217305 - vec.x;
		//				//	((void(__thiscall*)(Matrix34*, float))(FIX_ADDR(0x005D2340)))(&v43, a1a);
		//				//}

		//				//Vector3 v40{ 0.f, 1000.f, 0.f };

		//				//const float old_x = v40.x;
		//				//const float old_y = v40.y;
		//				//const float old_z = v40.z;

		//				//const float new_x = (v43.a.x * old_x) + (v43.b.x * old_y) + (v43.c.x * old_z) + v43.d.x;
		//				//const float new_y = (v43.a.y * old_x) + (v43.b.y * old_y) + (v43.c.y * old_z) + v43.d.y;
		//				//const float new_z = (v43.a.z * old_x) + (v43.b.z * old_y) + (v43.c.z * old_z) + v43.d.z;

		//				//v40.x = new_x;
		//				//v40.y = new_y;
		//				//v40.z = new_z;

		//				//float* pWaterCannonOrientation = (float*)((BYTE*)this + 0x14E8);
		//				//float* pWaterCannonElevation = (float*)((BYTE*)this + 0x14EC);

		//				//float fWaterCannonOrientationOld = *pWaterCannonOrientation;
		//				//float fWaterCannonElevationOld = *pWaterCannonOrientation;


		//				//auto pCoord = getTransform();
		//				//Vector3 vec2{ v40.x - pCoord->d.x, v40.y - pCoord->d.y, v40.z - pCoord->d.z };
		//				//Vector3 vec3{ (float)((float)(pCoord->a.y * vec2.y) + (float)(vec2.x * pCoord->a.x)) + (float)(pCoord->a.z * vec2.z),
		//				//	(float)((float)(pCoord->b.y * vec2.y) + (float)(vec2.x * pCoord->b.x)) + (float)(pCoord->b.z * vec2.z),
		//				//	(float)((float)(pCoord->c.y * vec2.y) + (float)(pCoord->c.x * vec2.x)) + (float)(pCoord->c.z * vec2.z) };

		//				//((void(__thiscall*)(CVehicle*, Vector3*, float))(FIX_ADDR(0xA927D0)))(this, &vec3, 2.f);
		//				//((void(__thiscall*)(CVehicle*, Vector3*, float, float))(FIX_ADDR(0xA91CA0)))(this, &vec3, 1.f, v37);


		//				//float v21 = *pWaterCannonElevation;
		//				//float v22 = -0.15000001;
		//				//if (v21 >= -0.15000001)
		//				//{
		//				//	v22 = 0.78539819;
		//				//	if (v21 <= 0.78539819)
		//				//		v22 = *pWaterCannonElevation;
		//				//}
		//				//*pWaterCannonElevation = v22;

		//				//auto pBone = &pSkel->m_pBones[j];
		//				//auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pBone->m_pParent->m_pParent->m_wBoneIndex);
		//				//pMtx->identityRot();
		//				//pMtx->a.x = cosf(-*pWaterCannonOrientation);
		//				//pMtx->a.y = -sinf(-*pWaterCannonOrientation);
		//				//pMtx->b.x = sinf(-*pWaterCannonOrientation);
		//				//pMtx->b.y = cosf(-*pWaterCannonOrientation);

		//				//pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pBone->m_pParent->m_wBoneIndex);
		//				//pMtx->identityRot();
		//				//pMtx->b.y = cosf(-*pWaterCannonElevation);
		//				//pMtx->b.z = -sinf(-*pWaterCannonElevation);
		//				//pMtx->c.y = sinf(-*pWaterCannonElevation);
		//				//pMtx->c.z = cosf(-*pWaterCannonElevation);

		//			}


		//			auto pDriver = *(BYTE**)((BYTE*)this + 0xFA0);
		//			if (pDriver) {
		//				if (((bool(__thiscall*)(void*))(FIX_ADDR(0x004C3020)))(pDriver)) {
		//					if (pWeapon) {
		//						if (isVehAttackPressed(pDriver)) {
		//							printf("gun fire\n");

		//							Matrix34 mtx;
		//							auto pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, j);

		//							mtx = *pMtx;

		//							auto pBone = pSkel->m_pBones[j].m_pParent;
		//							while (pBone) {
		//								pMtx = ((Matrix34 * (__thiscall*)(CVehicle * _a, int))(g_CDynamicEntity__getLocalMatrix))(this, pBone->m_wBoneIndex);
		//								((Matrix34 * (__thiscall*)(Matrix34*, Matrix34*))(FIX_ADDR(0x00448A20)))(&mtx, pMtx);
		//								//mtx.multBy(pMtx);
		//								pBone = pBone->m_pParent;
		//							}

		//							pMtx = getTransform();
		//							((Matrix34 * (__thiscall*)(Matrix34*, Matrix34*))(FIX_ADDR(0x00448A20)))(&mtx, pMtx);


		//							((char(__thiscall*)(void*, void* a3, Matrix34 * a4, int* a5, int a6, char a7, int a8, int a9, float a10, float a11))(FIX_ADDR(0x009A4570)))
		//								(pWeapon, this, &mtx, 0, 0, 0, 0, 0, 0.f, -1.f); // CWeapon::fireSingleBullet


		//						}

		//					}
		//				}

		//			}
		//			
		//			break;
		//		}
		//	}
		//}



	}


};

size_t g_checkForAllowFiring_origcall;

bool __cdecl checkForAllowFiring(CPed* pPed) {

	if (auto veh = g_pfnGetVehicle(pPed)) {
		if(auto params = g_extVehParams.getFromVehicle(veh)) {
			auto weapons = params->getCarWeapInfoInfo();
			for (size_t i = 0; i < sizeof weapons->m_slots / sizeof *weapons->m_slots; i++)
				if(weapons->m_slots[i].m_bActive)
					return false;
		}
	}

	return ((bool(__cdecl*)(CPed*))(g_checkForAllowFiring_origcall))(pPed);
}

#pragma region rocketVelocityFix


struct CEntity;

CVehicle* g_pEntity = nullptr;

struct CC {

	static size_t ms_sub_9D8A90_origcall;
	static size_t ms_setInitialVelocity_origcall;

	char sub_9D8A90(CVehicle* a2, char a3) {
		g_pEntity = a2;

		return ((char(__thiscall*)(CC*, CVehicle * a2, char a3))(ms_sub_9D8A90_origcall))(this, a2, a3);
	}

	void setInitialVelocity(Vector3* pVec) {
		if (g_pEntity) {
			Vector3 vec;
			g_pEntity->getVelocity(&vec);
			pVec->x += vec.x;
			pVec->y += vec.y;
			pVec->z += vec.z;
			g_pEntity = nullptr;
		}

		((void(__thiscall*)(CC*, Vector3 * pVec))(ms_setInitialVelocity_origcall))(this, pVec);

	}

};

size_t CC::ms_sub_9D8A90_origcall;
size_t CC::ms_setInitialVelocity_origcall;

void initRocketVelocitySpeed() {
	CC::ms_sub_9D8A90_origcall = setFnAddrInCallOpcode(FIX_ADDR(0x00A136A1), getThisCallAddr(&CC::sub_9D8A90));
	CC::ms_setInitialVelocity_origcall = setFnAddrInCallOpcode(FIX_ADDR(0x00A136BA), getThisCallAddr(&CC::setInitialVelocity));
}

#pragma endregion



size_t cc::CDmgManager__getEngineHealth_origcall;




void init() {
	//writeBYTE(FIX_ADDR(0x00D6A4A8), '5');
	//writeBYTE(FIX_ADDR(0x00D6A4B4), '5');

	//initRocketVelocitySpeed();

	//cc::CDmgManager__getEngineHealth_origcall= setFnAddrInCallOpcode(FIX_ADDR(0x00A0EBEC), getThisCallAddr(&cc::CDmgManager__getEngineHealth));
	//g_checkForAllowFiring_origcall = setFnAddrInCallOpcode(FIX_ADDR(0x00A0EC03), (size_t)checkForAllowFiring);

	//writeDWORD(FIX_ADDR(0x00DBA73C), getThisCallAddr(&cc::processCannon));

	//makeNop(FIX_ADDR(0x00C451CD), 0x8);
	//makeNop(FIX_ADDR(0x0061EF67), 0x8);

	//setFnAddrInCallOpcode(FIX_ADDR(0x00B32FFB), getThisCallAddr(&cc::sub1));
}

}

namespace testCol {

struct CC {
	int processCol(float fTimeStep, char bCanPostpone, int nTimeSlice) { return 0; }

	char CPhysical__mDC() { return 0; }
	int CPhysical__100() { return 0; } // ?

	float* CPhysical__m10C(float* a2) { return a2; }
	void CPhysical__m110() { }
	void CPhysical__m114(char) {}
	void CPhysical__m118() {}

	void joint__m4C(float) { }
	int phJoint1Dof__m4() { return 0; }
	float phJoint1Dof__m8(int) { return 0.f; }

};

void init() {
	//////writeDWORD(FIX_ADDR(0x00DBA680), getThisCallAddr(&CC::processCol));
	////writeDWORD(FIX_ADDR(0x00DBA658), getThisCallAddr(&CC::CPhysical__mDC));
	////writeDWORD(FIX_ADDR(0x00DBA67C), getThisCallAddr(&CC::CPhysical__100));
	//////writeDWORD(FIX_ADDR(0x00DBA688), getThisCallAddr(&CC::CPhysical__m10C));
	//////writeDWORD(FIX_ADDR(0x00DBA688 + 4), getThisCallAddr(&CC::CPhysical__m110));
	////writeDWORD(FIX_ADDR(0x00DBA688 + 8), getThisCallAddr(&CC::CPhysical__m114));
	////writeDWORD(FIX_ADDR(0x00DBA688 + 0xC), getThisCallAddr(&CC::CPhysical__m118));

	//writeDWORD(FIX_ADDR(0x00732310) + 4, 0xB0);

	size_t vmt = 0x00EB3C94;
	//writeDWORD(FIX_ADDR(0x00EB3CE0), getThisCallAddr(&CC::joint__m4C));
	////writeDWORD(FIX_ADDR(vmt + 0x4), getThisCallAddr(&CC::phJoint1Dof__m4));
	//writeDWORD(FIX_ADDR(vmt + 0x8), getThisCallAddr(&CC::phJoint1Dof__m8));
}
}

namespace newWeaponInfoIds {

uint32_t g_weapHashId[0xFF];
uint32_t g_numWeapHashId = 0;

uint32_t *g_origHashes = nullptr;

void initNewWeaponHashes() {

	do {
		g_weapHashId[g_numWeapHashId++] = *g_origHashes;
		g_origHashes++;
	} while (*g_origHashes);

}

int __cdecl getWeaponInfoIdByHash(int a1, int a2 = 55) {
	if (g_numWeapHashId == 0) {
		initNewWeaponHashes();
	}

	for (size_t i = 0; i < g_numWeapHashId; i++)
		if (a1 == g_weapHashId[i])
			return i;

	
	if (a2 <= 0) {
		a2 = g_numWeapHashId;
		g_weapHashId[g_numWeapHashId++] = a1;
		printf("added %u at %i\n", a1, a2);
	}

	return a2;
}



struct CWeaponInfo {
	uint8_t __0[0x110];
};

CWeaponInfo g_weapInfo[0xFF];

CWeaponInfo* __cdecl getWeaponInfo(int a1) {
	if (a1 >= g_numWeapHashId)
		return g_weapInfo;
	else
		return &g_weapInfo[a1];
}

void init() {
	g_origHashes = *(uint32_t**)(FIX_ADDR(0x00D3C166) + 1);

	injectFunc(FIX_ADDR(0x009AB640), (size_t)getWeaponInfoIdByHash);
	injectFunc(FIX_ADDR(0x009AB620), (size_t)getWeaponInfo);

	//g_weapInfo = new CWeaponInfo[0xFF];

	writeDWORD(FIX_ADDR(0x009AB635) + 1, (size_t)g_weapInfo);
	writeDWORD(FIX_ADDR(0x009AB62F) + 1, (size_t)g_weapInfo);
	writeDWORD(FIX_ADDR(0x009ACB43) + 2, (size_t)g_weapInfo);
	writeDWORD(FIX_ADDR(0x00D3C121) + 1, (size_t)g_weapInfo);

	writeDWORD(FIX_ADDR(0x00D3C126) + 1, 0xFF);
	//writeBYTE(FIX_ADDR(0x009AB624) + 2, 0xFF);

}

}

namespace spawnWeapon {

size_t g_processGame_origcall;
__forceinline bool __cdecl isGameKeyboardKeyJustPressed(int a1) {
	//return ((bool(__cdecl*)(int a1))(FIX_ADDR(0x00C187F0)))(a1);
	return ((bool(__cdecl*)(int a1))(FIX_ADDR(0x00B98CD0)))(a1);
}


char processGame() {
	if (isGameKeyboardKeyJustPressed(38)) {
		printf("weapon\n");

		atPool<CPed>* pool = *(atPool<CPed>**)FIX_ADDR(0x018B6F1C);
		//atPool<CPed>* pool = *(atPool<CPed>**)FIX_ADDR(0x018219EC);

		for (size_t i = 0; i < pool->m_dwSize; i++) {
			if (auto ped = pool->at(i)) {
				if (((bool(__thiscall*)(void*))(g_CPed__getIsPlayer))(ped)) {

					
					//auto id = ((int(__cdecl*)(int a, int b))(getFnAddrInCallOpcode(FIX_ADDR(0xAA6C95E5))))(0xB4A54CDA, 55);
					auto id = ((int(__cdecl*)(int a, int b))(getFnAddrInCallOpcode(FIX_ADDR(0x00D9E31B))))(0xB4A54CDA, 55);
					//auto id = ((int(__cdecl*)(int a, int b))(getFnAddrInCallOpcode(FIX_ADDR(0x00D9E31B))))(0x625E70F9, 55);

					if (id == 55) {
						MessageBoxA(nullptr, "unk weapon", nullptr, 0x10);
					}
					else {
						((char(__thiscall*)(void*, int, WORD, char, char, char))(FIX_ADDR(0x00B64F70)))((BYTE*)ped + 0x2B0, id, 8, 1, 0, 0);

						printf("spawn\n");
					}
				}
			}
		}
	}

	return ((char(*)())(g_processGame_origcall))();

}

void init() {
	g_processGame_origcall = setFnAddrInCallOpcode(FIX_ADDR(0x0059D7D8), (size_t)processGame);
	//g_processGame_origcall = setFnAddrInCallOpcode(FIX_ADDR(0x0047364D), (size_t)processGame);
}

}

namespace explore {

void __cdecl test() {

}
}

void runTest() {
	//spawnWeapon::init();
	//newWeaponInfoIds::init();

	//test4::init();

	/*
	//setFnAddrInCallOpcode(FIX_ADDR(0x00B283E0), getThisCallAddr(&__::sub_1));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00B283F1), getThisCallAddr(&__::sub_1));
	//setFnAddrInCallOpcode(FIX_ADDR(0x00B283F9), getThisCallAddr(&__::sub_1));
	setFnAddrInCallOpcode(FIX_ADDR(0x00B28419), getThisCallAddr(&__::sub_1));

	//setFnAddrInCallOpcode(FIX_ADDR(0x00B2961A), getThisCallAddr(&__::sub));
	setFnAddrInCallOpcode(FIX_ADDR(0x00B28411), getThisCallAddr(&__::sub));
//	setFnAddrInCallOpcode(FIX_ADDR(0x00B28409), getThisCallAddr(&__::sub));
	setFnAddrInCallOpcode(FIX_ADDR(0x00B28421), getThisCallAddr(&__::sub));

//	reload::init();
	//init();

//	doubleSidemarkFix::init();
*/
	//pedNormalMapBlend::init();
	//moreWheels::init();

//	wheelVfx::init();
//	testCol::init();

	//CustomVehFxCheckFix::patch();

	//void* addr_gAssetManager = *(void**)(FIX_ADDR(0x007CB7D2) + 2);
	//size_t addr_fiAssetMgr__expandPath = FIX_ADDR(0x0045AD60);

	//((BYTE*(__thiscall*)(void* _a, const char* pDst, int nLen, const char* pName, const char* pExt, int a6))(CVehicle__prerender_origcall))(_a);

	//writeDWORD(FIX_ADDR(0x00B206D7) + 2, 0);
	//writeDWORD(FIX_ADDR(0x00B206BE) + 2, 0);
	//writeDWORD(FIX_ADDR(0x00B2070A) + 2, 0);

	//makeNop(FIX_ADDR(0x00B20708), 0x8);

	//tVehAud::init();

	//if (CConfig::ms_bPatchEngine) {
	//	//setFnAddrInCallOpcode(g_hookAddr_CTransmission__process, getThisCallAddr(&gears::CTransmission::process));

	//	writeDWORD(g_CAutomobile__processPhysics_engineOnCheckJump, 0);

	//	//if (CConfig::ms_bEngineComponents) {
	//	//	g_pfnVehPrerenderAddon[g_numVehPrerenderAddon++] = prerenderEngineComponents;

	//	//	g_pfnVehProcessControlAddon[g_numVehProcessControlAddon++] = updateEngineComponents;
	//	//}

	//	//if (CConfig::ms_bPatchCustomVehFX) {
	//	//	g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnInit = initEngineVars;
	//	//	g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnSetVars = setVarsEngineVars;
	//	//	g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnUpdate = updateEngineVars;
	//	//	g_numCustomVehFxAddons++;
	//	//}
	//}

	printDebug("test zone\n");
}