#include "addrs.h"
#include "CHandling.h"
#include "CVehicle.h"
#include "vehExtParams.h"
#include "helpers.h"
#include "hookFns.h"
#include "fx.h"

namespace doubleSidemarkFix {

struct phMaterial {
	void* __vmt;

	uint32_t _f4;
	const char* m_pszName;
	float m_fFriction;
	float m_fElasticity;
};

struct phMaterialGta : phMaterial {
	float m_fDensity;
	float m_fTyreGrip;
	float m_fWetGrip;
	uint16_t m_wFxGroup;
	uint16_t m_wHeliFx; // wtf!? ToDo: m_wFxHash
	float m_fFlammability;
	float m_fBurnTime;
	float m_fBurnStrenght;
	int32_t m_nRoughness;
	float m_fPedDensity;
	uint32_t m_dwFlags;
};

struct phMaterialMgrGta {

	virtual ~phMaterialMgrGta() { }

	virtual int32_t initialize(uint32_t a2) { return 0; }
	virtual int32_t terminate() { return 0; }
	virtual int32_t getMaterialCount() { return 0; }
	virtual int32_t getMaterialByName(const char* a2) { return 0; }
	virtual phMaterialGta* getMaterialByIndex(uint8_t a2) { return 0; }
	virtual int32_t getMaterialIndexByName(const char* a2) { return 0; }
	virtual uint8_t* getMaterialName(const char* dwMaterialIndex, uint8_t* pszName, int32_t dwMNameLength) { return nullptr; }
	virtual int32_t getMaterialIndex(phMaterialGta* a2) { return 0; }
	virtual int32_t getDefaultMaterial() { return 0; }
	virtual uint32_t* m28(uint32_t* a1, int32_t a2, int32_t a3) { return nullptr; }
	virtual int32_t m2C(int32_t a1) { return 0; }
	virtual uint8_t* addMaterial(const char* dwMaterialIndex, char* a2) { return nullptr; }
	virtual uint8_t* getMaterialByIndex_1(uint32_t a2) { return nullptr; }
	virtual uint32_t* getMaterialIndexByName_1(char* a2) { return nullptr; }
	virtual void m3C() { }
	virtual void pad40() { }
	virtual void pad44() { }

};




struct CWheel_fixed : CWheel {

	static float ms_fWetroadWetMinThresh;

	void processSkidmarkVFx(CVehicle* pParentVehicle, float currRoadWetness, char bUseMaterialCarPlasticFxGroup, float fDistScale) {
		if (getDistSqrToCamera(&m_vHitCentrePos) >= fDistScale * 900.f)
			return; // Занадто далеко, нічого не робити

		phMaterialMgrGta* g_pMaterialMgr = *(phMaterialMgrGta**)g_ppMaterialMgr;

		CVehicleFx* g_vehFx = (CVehicleFx*)g_pVehFx;


		// Checking the properties of the material
		bool isMaterialOk = (g_pMaterialMgr->getMaterialByIndex(m_dwHitMaterialId)->m_dwFlags & 2) == 0 && (m_dwHitMaterialId & 0x8000000) == 0;

		if (!isMaterialOk)
			return; // The material is not suitable for creating a skidmark

		bool bTank = false;
		if (auto pParams = g_extVehParams.getFromVehicle(pParentVehicle)) {
			if(pParams->getUseTankSkidmark())
				bTank = true;
		}


		float oldHealth;
		if (bTank) {
			oldHealth = m_fTyreHealth;
			m_fTyreHealth = 0.f;
		}

		FxTyreState_e state = FX_TYRE_STATE_HEALTHY_TIRE_DRY_ROAD;
		if (m_fTyreHealth == 0.f) { // if tire health is 0 (worn tire)
			if (currRoadWetness < ms_fWetroadWetMinThresh) // VFXWHEEL_WETROAD_WET_MIN_THRESH is 0.60000002f
				state = FX_TYRE_STATE_WORN_TIRE_DRY_ROAD; // Worn tire, dry road
			else
				state = FX_TYRE_STATE_WORN_TIRE_WET_ROAD; // Worn tire, wet road
		}
		else if (currRoadWetness >= ms_fWetroadWetMinThresh) // Healthy tire, wet road
			state = FX_TYRE_STATE_HEALTHY_TIRE_WET_ROAD;

		uint32_t materialFxGroup = 0;
		uint32_t materialFxGroup2 = 0; // Wtf?

		if (bUseMaterialCarPlasticFxGroup) {
			materialFxGroup2 = 34;
			materialFxGroup = 34;
		}
		else {
			// Getting material properties for effects
			materialFxGroup = g_pMaterialMgr->getMaterialByIndex(m_dwHitMaterialId)->m_wFxGroup;
			materialFxGroup2 = g_pMaterialMgr->getMaterialByIndex(m_dwHitMaterialId)->m_wFxGroup;
		}

		VehicleFxInfo_s* wheelInfo = g_vehFx->getInfo(state, materialFxGroup2);
		float* pSkidmarkThresh = &wheelInfo->m_fSkidmarkThresh;
		float skidmarkThresh = wheelInfo->m_fSkidmarkThresh;
		bool bIsLiquidSkidmark = false; // bIsLiquidSkidmark

		//if (((m_dwFlags & 0x10) != 0 || (m_dwFlags & 0x20) != 0) && m_fTyreHealth > 0.0f) {
		//	skidmarkThresh = 0.1f; /в
		//	bIsLiquidSkidmark = true;
		//}

		float skidmarkIntensity = m_fSkidmarkIntensity;

		float absSkidmarkIntensity = (skidmarkIntensity >= 0.f) ? skidmarkIntensity : -skidmarkIntensity; // ToDo: fabsf(m_fSidemarkIntensity)

		if (absSkidmarkIntensity > skidmarkThresh) {
			float effectIntensity = 0.f; // v33

			if ((m_dwFlags & 0x10) != 0 || (m_dwFlags & 0x20) != 0) {
				effectIntensity = _f168; // ???
				if (effectIntensity < 1.f) {
					if (effectIntensity <= 0.f)
						effectIntensity = 0.f;
				}
				else
					effectIntensity = 1.f;
			}
			else {
				int32_t materialFxGroupResult = g_pMaterialMgr->getMaterialByIndex(m_dwHitMaterialId)->m_wFxGroup;
				if (materialFxGroupResult == 8 || materialFxGroupResult == 6 || materialFxGroupResult == 23) {
					effectIntensity = 1.f;
				}
				else {
					effectIntensity = (fabsf(m_fSkidmarkIntensity) - *pSkidmarkThresh) * 0.25f;
					if (effectIntensity >= 1.f)
						effectIntensity = 1.f;
					else if (effectIntensity <= 0.f)
						effectIntensity = 0.f;
				}
			}

			// Calculating speed and friction
			Matrix34* pCoords = pParentVehicle->getTransform();

			float dotProductVelocity = (m_vHitNormal.z * pCoords->c.z) +
				(m_vHitNormal.y * pCoords->c.y) +
				(m_vHitNormal.x * pCoords->c.x);

			float dword_F36A3C = 0.5f;
			if (dword_F36A3C <= dotProductVelocity) {
				float velocitySquared = dotProductVelocity * dotProductVelocity; // v38

				if (m_fFrictionDamage <= 0.5f) {
					//*((uint8_t*)pParentVehicle + 0x134C) = 1;
				}

				Vector3 normalizedGroundVelocity = m_vGroundVelocity;
				normalizedGroundVelocity.normalize(); // Нормалізація вектора

				float sideSlipFactor = (pCoords->b.z * normalizedGroundVelocity.z) +
					(pCoords->b.y * normalizedGroundVelocity.y) +
					(pCoords->b.x * normalizedGroundVelocity.x);
				if (sideSlipFactor < 0.f)
					sideSlipFactor = -sideSlipFactor; // ToDo: fabsf(sideSlipFactor)

				float skidmarkBlendFactor;
				if (m_fTyreHealth == 0.f)
					skidmarkBlendFactor = (sideSlipFactor * sideSlipFactor) * 0.94999999f + 0.050000001f;
				else
					skidmarkBlendFactor = (sideSlipFactor * 0.5f) + 0.5f;


				TrailInfo_t trailInfo;
				trailInfo._f4 = 0;
				trailInfo._fC = m_pReference;
				trailInfo._f10 = _fdc;
				trailInfo._f20.x = m_vHitCentrePos.x;
				trailInfo._f20.y = m_vHitCentrePos.y;
				trailInfo._f20.z = m_vHitCentrePos.z;
				trailInfo._f20.w = m_vHitCentrePos.w;
				trailInfo._f80 = (m_fWheelWidth + 0.06f) * skidmarkBlendFactor * velocitySquared;
				trailInfo._f88 = bIsLiquidSkidmark;
				trailInfo._f89 = 0;
				trailInfo._f8A = 0;
				trailInfo._f8C = pSkidmarkThresh;
				trailInfo._f98 = 0;
				trailInfo._f70.x = m_vHitNormal.x;
				trailInfo._f70.y = m_vHitNormal.y;
				trailInfo._f70.z = m_vHitNormal.z;
				trailInfo._f70.w = m_vHitNormal.w;
				trailInfo._f0 = reinterpret_cast<int32_t>(this);
				memset(&trailInfo._f60, 0, sizeof trailInfo._f60);
				trailInfo._f84 = effectIntensity;

				auto sub_7FAA80 = ((void(__cdecl*)(CVehicle * a1,
					Vector4 * a2,
					Vector4 * a3,
					float a4,
					float a5,
					char a6,
					char a7,
					__int16 a8,
					char a9,
					char a10,
					char a11))(g_unkVisualEffectFn1));

				uint8_t* pByte_1174B20 = (uint8_t*)g_pUnkVisualEffectObject;

				auto sub_8BBC20 = ((void(__thiscall*)(void* a1, TrailInfo_t*, char a3))(g_unkVisualEffectFn2));


				// Calling the main Vfx creation function
				sub_7FAA80(pParentVehicle, &trailInfo._f20, &trailInfo._f70, trailInfo._f80, trailInfo._f84, materialFxGroup,
					state, _fdc, m_dwBoneId, 0, bIsLiquidSkidmark);

				sub_8BBC20(pByte_1174B20, &trailInfo, 0);

				if (g_pfnGetVehSpecialType(pParentVehicle) != 1 && // If the vehicle type is not a bike
					(m_dwFlags & 0x1000) != 0 && // If it is the rear wheel
					(m_pHandling->m_dwModelFlags & MODELFLAG_DOUBLE_R_WHEELS) != 0) {

					float wheelWidth = m_fWheelWidth;
					trailInfo._f0 = reinterpret_cast<int32_t>(&m_dwBoneId) + 1; // wtf?

					float offset_x = (pCoords->a.x * wheelWidth) * 1.1f;
					float offset_y = (pCoords->a.y * wheelWidth) * 1.1f;
					float offset_z = (pCoords->a.z * wheelWidth) * 1.1f;

					// Left
					if ((m_dwFlags & 0x800) == 0) {
						trailInfo._f20.y -= offset_y;
						trailInfo._f20.x -= offset_x;
						trailInfo._f20.z -= offset_z;
					}
					else { // Right
						trailInfo._f20.x += offset_x;
						trailInfo._f20.y += offset_y;
						trailInfo._f20.z += offset_z;
					}

					// Calling functions for the additional tire skidmark
					sub_7FAA80(pParentVehicle, &trailInfo._f20, &trailInfo._f70, trailInfo._f80, trailInfo._f84, materialFxGroup, state, _fdc, m_dwBoneId, 1, bIsLiquidSkidmark);
					sub_8BBC20(pByte_1174B20, &trailInfo, 0);
				}
			}
		}
		if (bTank)
			m_fTyreHealth = oldHealth;
	}

};

float CWheel_fixed::ms_fWetroadWetMinThresh = 0.6f;

void init() {
	setFnAddrInCallOpcode(g_hookAddr_CWheel__processSkidmarkVFx, getThisCallAddr(&CWheel_fixed::processSkidmarkVFx));
}

} // namespace doubleSidemarkFix