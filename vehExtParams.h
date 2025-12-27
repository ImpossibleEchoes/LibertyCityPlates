#pragma once
#include <windows.h>
#include "trace.h"
#include "weapon.h"

#include <iostream>
struct CVehicle;
struct CVehStruct2;
struct CWheel;

#ifdef _DEBUG 
#define CHECK_ADDR(offset)\
		if (offset == -1)\
			printDebug("[%s] offset == -1\n", __FUNCSIG__ );
#else
#define CHECK_ADDR(offset)
#endif // _DEBUG

extern struct CVehExtParams {

//#define GET_ADDR(offset) (static_cast<void*>(reinterpret_cast<uint8_t*>(this) + (offset)))

public:
	struct CTankTrackParams {
		float m_TrackUV[2]; // 0..1
		float m_TrackDriveWheelScale[2];
		float m_TrackDriveWheelRot[2];
	};

	struct CPlateInfo {
		Vector4 m_vecPlateTextTint; // align to 0x10
		Vector4 m_vecLettersScaleAndOffset; // xy = scale, zw - offset
		uint8_t m_aLicensePlateText[12];
		uint8_t m_NumChars;
		uint8_t m_PlateTexIndex; // if m_nbPlateTexIndex lt g_nbPlateTexCount then m_nbPlateTexIndex = g_nbPlateTexCount - 1
		uint8_t m_LettersTexIndex; //

	};

	struct CVehicleWeapon {
		struct Slot {
			CWeapon* m_pWeapon;
			int32_t m_aBones[MAX_NUM_CAR_WEAPONS_BONES];
			uint8_t m_CurBone;
			uint8_t m_NumBones;
			bool m_bActive;

			void setDefaultParams() {
				//Slot() {
				m_pWeapon = nullptr;
				m_NumBones = 0;
			}

			~Slot() {
				if (m_pWeapon) {
					delete m_pWeapon;
					m_pWeapon = nullptr;
				}
				m_bActive = false;
			}

			void initialize(CCarWeapInfo::Weapon* pCarWeapInfo) {
				if (pCarWeapInfo->m_weapId == -1)
					return;
				m_pWeapon = new CWeapon;
				m_pWeapon->initialize(pCarWeapInfo->m_weapId, 25000, false);
				m_NumBones = pCarWeapInfo->m_numBones;
				m_CurBone = 0;
				for (size_t i = 0; i < m_NumBones; i++)
					m_aBones[i] = pCarWeapInfo->m_aBones[i];

				m_bActive = true;
			}
		};

		Slot m_slots[MAX_NUM_CAR_WEAPONS];
		float m_fTurret1Orientation;
		float m_fTurret1Elevation;
		float m_fTurret2Orientation;
		float m_fTurret2Elevation;

		void initialize(CCarWeapInfo* pCarWeapInfo) {
			for (size_t i = 0; i < MAX_NUM_CAR_WEAPONS; i++)
				m_slots[i].initialize(&pCarWeapInfo->m_aWeapons[i]);
		}

		void setDefaultParams() {
			//CVehicleWeapon() {
			for (size_t i = 0; i < MAX_NUM_CAR_WEAPONS; i++)
				m_slots[i].setDefaultParams();
			m_fTurret1Orientation = 0.f;
			m_fTurret1Elevation = 0.f;
			m_fTurret2Orientation = 0.f;
			m_fTurret2Elevation = 0.f;
		}
	};

	const static size_t ms_alignas = 0x100;

	struct alignas(0x10) Params {

		template <typename T> __forceinline T* getAddr(size_t offset) {
			return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(this) + offset);
		}


		//static size_t ms_charsOffset; // Now it in CPlateInfo
		static size_t ms_structOffset;
		static size_t ms_size;
		static size_t ms_speedArrowOffset;
		static size_t ms_rpmArrowOffset;
		static size_t ms_tempArrowOffset;
		static size_t ms_engineRotOffset;
		static size_t ms_superchargerThrottleRotOffset;
		static size_t ms_tankTrackParamsOffset;
		static size_t ms_moreWheelsOffset;
		static size_t ms_useTankSkidmarkOffset;
		static size_t ms_vehWeapOffset;
		static size_t ms_plateInfoOffset;


		//__forceinline uint8_t* getPlateChars() {
		//	CHECK_ADDR(ms_charsOffset);
		//	return getAddr<uint8_t>(ms_charsOffset);
		//}

		__forceinline CVehStruct2* getStruct() {
			CHECK_ADDR(ms_structOffset);
			return *getAddr<CVehStruct2*>(ms_structOffset);
		}

		__forceinline void setStruct(CVehStruct2* pStruct) {
			CHECK_ADDR(ms_structOffset);
			*getAddr<CVehStruct2*>(ms_structOffset) = pStruct;
		}

		__forceinline float getSpeedArrow() {
			CHECK_ADDR(ms_speedArrowOffset);
			// Отримуємо float* та розіменовуємо його
			return *getAddr<float>(ms_speedArrowOffset);
		}

		__forceinline void setSpeedArrow(float val) {
			CHECK_ADDR(ms_speedArrowOffset);
			*getAddr<float>(ms_speedArrowOffset) = val;
		}

		__forceinline float getRpmArrow() {
			CHECK_ADDR(ms_rpmArrowOffset);
			return *getAddr<float>(ms_rpmArrowOffset);
		}

		__forceinline void setRpmArrow(float val) {
			CHECK_ADDR(ms_rpmArrowOffset);
			*getAddr<float>(ms_rpmArrowOffset) = val;
		}

		__forceinline float getTempArrow() {
			CHECK_ADDR(ms_tempArrowOffset);
			return *getAddr<float>(ms_tempArrowOffset);
		}

		__forceinline void setTempArrow(float val) {
			CHECK_ADDR(ms_tempArrowOffset);
			*getAddr<float>(ms_tempArrowOffset) = val;
		}

		__forceinline float getEngineRot() {
			CHECK_ADDR(ms_engineRotOffset);
			return *getAddr<float>(ms_engineRotOffset);
		}

		__forceinline void setEngineRot(float val) {
			CHECK_ADDR(ms_engineRotOffset);
			*getAddr<float>(ms_engineRotOffset) = val;
		}

		__forceinline float getSuperchargerThrottleRot() {
			CHECK_ADDR(ms_superchargerThrottleRotOffset);
			return *getAddr<float>(ms_superchargerThrottleRotOffset);
		}

		__forceinline void setSuperchargerThrottleRot(float val) {
			CHECK_ADDR(ms_superchargerThrottleRotOffset);
			*getAddr<float>(ms_superchargerThrottleRotOffset) = val;
		}

		__forceinline CTankTrackParams* getTankTrackInfo() {
			CHECK_ADDR(ms_tankTrackParamsOffset);
			return getAddr<CTankTrackParams>(ms_tankTrackParamsOffset);
		}

		__forceinline CWheel* getMoreWheels() {
			CHECK_ADDR(ms_moreWheelsOffset);
			return getAddr<CWheel>(ms_moreWheelsOffset);
		}

		__forceinline bool getUseTankSkidmark() {
			CHECK_ADDR(ms_useTankSkidmarkOffset);
			return *getAddr<bool>(ms_useTankSkidmarkOffset);
		}

		__forceinline void setUseTankSkidmark(bool val) {
			CHECK_ADDR(ms_useTankSkidmarkOffset);
			*getAddr<bool>(ms_useTankSkidmarkOffset) = val;
		}

		__forceinline CVehicleWeapon* getCarWeapInfoInfo() {
			CHECK_ADDR(ms_vehWeapOffset);
			return getAddr<CVehicleWeapon>(ms_vehWeapOffset);
		}

		__forceinline CPlateInfo* getPlateInfo() {
			CHECK_ADDR(ms_plateInfoOffset);
			return getAddr<CPlateInfo>(ms_plateInfoOffset);
		}

		void destroy() {
			if (ms_vehWeapOffset != -1) {
				auto pWeap = getCarWeapInfoInfo();
				for (size_t i = 0; i < MAX_NUM_CAR_WEAPONS; i++)
					pWeap->m_slots[i].~Slot();
			}
		}

		void setDefaultParams() {
			memset(this, 0, ms_size);
			if (ms_vehWeapOffset != -1) {
				auto pWeap = getCarWeapInfoInfo();
				pWeap->setDefaultParams();
			}
		}

	};

	bool resize(size_t newSize);
	bool resizeNoAlign(size_t newSize);

	Params* m_pParams;
	size_t m_size;

	CVehicle* m_pVehMemStart;
	CVehicle* m_pVehMemEnd;

	Params* getFromVehicle(CVehicle* veh) noexcept;
	Params* getByIndex(int32_t index) noexcept;

	static void init();

	CVehExtParams() : m_pParams(nullptr), m_size(0), m_pVehMemStart(nullptr), m_pVehMemEnd(nullptr) {}

} g_extVehParams;

struct CVehicleFactory {

	static size_t ms_createVehicle_origcall;

	CVehicle* createVehicle(uint32_t dwModel, uint32_t createdBy, void* pMtx, BOOL bNetwork);
	static void init();
};


extern void(__cdecl* g_pfnVehFactoryAddon[0x10])(CVehicle*, CVehExtParams::Params*);
extern uint8_t g_numVehFactoryAddon;


struct grcTexture;
struct grmShaderGroup;

void rage__grmShaderGroup__setVarVector4(grmShaderGroup* pShaderGroup, int32_t groupIndex, float* pVec);
void rage__grmShaderGroup__setVarTexture(grmShaderGroup* pShaderGroup, int32_t groupIndex, grcTexture* pTxd);
uint32_t rage__grmShaderGroup__addShaderGroupVar(grmShaderGroup* pShaderGroup, const char* pszName, bool bRequired);

struct __declspec(align(4)) CCustomShaderEffectVehicleFX {
	size_t __vmt;
	int16_t m_wSize;
	uint8_t __4[12 - 2];
	float m_fPrimaryColor[3];
	uint8_t m_nPrimaryColor[4];
	float m_fSecondaryColor[3];
	uint8_t m_nSecondaryColor[4];
	float m_fTertiaryColor[3];
	uint8_t m_nTertiaryColor[4];
	float m_fQuaternaryColor[3];
	uint8_t m_nQuaternaryColor[4];
	float m_fQuinaryColor[3];
	uint8_t m_nQuinaryColor[4];
	float m_fSpecularColor[3];
	uint8_t m_nSpecularColor[4];
	grmShaderGroup* m_pShaderGroup;
	float m_fDirtLevel;
	uint8_t m_bWrecked;
	float m_fSpecular;
	float m_fSpecularColor__;
	float m_fSpecular2Factor;
	float m_fSpecular2FactorIntensity;
	float m_fReflectivity;
	float field_90;
	float field_94;
	float field_98;
	float field_9C;
	float field_A0;
	float field_A4;
	float field_A8;
	float field_AC;
	float field_B0;
	float field_B4;
	float field_B8;
	float field_BC;
	float field_C0;
	float field_C4;
	float field_C8;
	int8_t field_CC;
	int8_t m_bSwitchOn;
	float m_dBoundRadius;
	grcTexture* m_pDamageTexture;
	int8_t m_nLivery;
	uint32_t field_DC;
	int16_t field_E0;
	int8_t field_E2;
	int8_t field_E3;
	int8_t m_nbShadersCount;
	uint32_t m_dwSpecular2ColorIndex;
	uint32_t m_dwDiffuseColor2Index;
	uint32_t m_dwDirtLevelIndex;
	uint32_t m_dwDimmerSetIndex;
	uint32_t m_dwDiffuseTex2Index;
	uint32_t m_dwTyreDeformSwitchOnIndex;
	uint32_t m_dwSwitchOnIndex;
	uint32_t m_dwBoundRadiusIndex;
	uint32_t m_dwDamageTextureIndex;
	uint32_t m_dwSpecularIndex;
	uint32_t m_dwSpecularColorIndex;
	uint32_t m_dwSpecular2FactorIndex;
	uint32_t m_dwSpecular2FactorIntensityIndex;
	uint32_t m_dwReflectivityIndex;
	uint32_t field_120;
	uint32_t field_124;
	uint32_t field_128;
	uint32_t field_12C;

	struct alignas(0x10) CAddonVars {
		//int32_t m_nCringeLevelIndex;
		//float m_fCringeLevel;

		float m_fTrack2; // +0
		int32_t m_nLetters0123Index; // +4
		int32_t m_nLetters4567Index; // +8
		int32_t m_nLetters891011Index; // +C
 
		float m_fTrack[2]; // +10
		int32_t m_nTrack2Index; // +18
		int32_t m_nTrackIndex; // +1C

		alignas(0x10) float m_aLetters0123[4]; // +20
		float m_aLetters4567[4]; // +30
		float m_aLetters891011[4]; // +40

		int32_t m_nLicensePlateValuesIndex; // +50
		int32_t m_nLettersScaleIndex; // +54
		int32_t m_nLettersOffsetIndex; // +58
		int32_t m_nPlateTxdId; // +5C
		int32_t m_nLettersTxdId; // +60
		int32_t m_nLettersTintIndex; // +64

		int32_t m_nLettersTxdDifIndex; // +68
		int32_t m_nLettersTxdNrmIndex; // +6C

		grcTexture *m_pPlate_d;
		grcTexture *m_pPlate_n;
		grcTexture *m_pPlate_s;
		grcTexture *m_pLetters_d;
		grcTexture *m_pLetters_n;

		alignas(0x10) float m_aLicensePlateValues[4]; // +70
		float m_aLettersScale[4]; // +80
		float m_aLettersOffset[4]; // +90
		float m_aLettersTint[4]; // +A0

		int32_t m_nPlateTxdDifIndex; // +B0
		int32_t m_nPlateTxdNrmIndex; // +B4
		int32_t m_nPlateTxdSpecIndex; // +B8

	};

	__forceinline CAddonVars* getAddonVars() { return (CAddonVars*)((uint8_t*)this + m_wSize - sizeof CAddonVars); }

	static void patchSize();

	static size_t ms_updateOrigcall;
	static size_t ms_initOrigcall; //
	static size_t ms_setVarsOrigcall; //

	char update(CVehicle* pVeh);
	char init(void* pDrawableRef);
	void setVars();

	static void patchVehCheck();
	static void initHooks();
};

struct CCustomShaderVehFXAddons {
	void(__cdecl* m_pfnUpdate)(CCustomShaderEffectVehicleFX*, CVehicle*, CVehExtParams::Params*, CCustomShaderEffectVehicleFX::CAddonVars*);
	void(__cdecl* m_pfnSetVars)(CCustomShaderEffectVehicleFX*, CCustomShaderEffectVehicleFX::CAddonVars*);
	void(__cdecl* m_pfnInit)(CCustomShaderEffectVehicleFX*, CCustomShaderEffectVehicleFX::CAddonVars*);
};

extern CCustomShaderVehFXAddons g_customVehFxAddons[0x10];
extern uint8_t g_numCustomVehFxAddons;

struct native_obj;

	// new commands:
// SET_CAR_LICENSE_PLATE_TEXT_0123 = 0x09ded943
// SET_CAR_LICENSE_PLATE_TEXT_4567 = 0x09de9587
// SET_CAR_LICENSE_PLATE_TEXT_891011 = 0x0e515a91


class vehicleScript {

private:

	static void __cdecl registerNewCommands();
	static char __cdecl setupScriptCommands_hook(uint32_t hash, size_t pfn);

public:

	static void init();

};



