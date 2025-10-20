#pragma once
#include <Windows.h>
#include "rage_math.h"

struct CVehicle {

	__forceinline uint16_t getModelIndex() { return *reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(this) + 0x2E); }
	__forceinline Vector3* getVelocity(Vector3* pVec) { return ((Vector3 * (__thiscall*)(CVehicle*, Vector3*))(*(void***)this)[59])(this, pVec); }
	__forceinline Matrix34* getTransform() { return *reinterpret_cast<Matrix34**>(reinterpret_cast<uint8_t*>(this) + 0x20); }

};

struct CHandlingVehicle;

struct CWheel {

	uint32_t m_dwBoneId;					// 000-004 in the order from tVehicleStruct
	uint16_t m_wFragChild;				// 004-006 group for detach, needs to be higher than 0
	uint16_t pad;						// used if flags._f11 != 0
	float m_fWheelRadius;							//                        ?                                            
	float m_fDiscRadius;							//                  veh.ide
	float m_fWheelWidth;							// Margin     AABB * 2.              AABB       . 
	CHandlingVehicle* m_pHandling;						//        ?
	float _f18;							//                                        (r   r, f   f )
	float _f1c;							// 0.38? posZ?
	float _f20; // rotation, for hydraulic							//                        ?
	float m_fTyreStability;			// m_fSusStiffnessCoefficient?				//                    ?                        ?
	float m_fTyreStabilityPercent;							// 1/_f24 
	float _f2c;							//            x? 1   l           -1   r						                          
	Vector3 matrix_right;				//           y ? 1.                  (         ).                    . 1
	Vector3 matrix_at;					//                

	Vector3 m_vCollisionOffset;
	Vector3 m_vModelOffset;

	float m_fCompression;							// default = -101
	float _f74;							// m_fCompression2? default = -101
	float m_fTyreWearRate;			// m_fSusLength?				
	float m_fRotationX;
	float m_fRpm;						//                  ?
	float m_fRpmOld;							//                  ?
	float m_fRotSlipRatio;							//         ,                            
	float m_fTyreTemp; // time on fire?
	Vector3 _f90;
	Vector3 m_vHitCentrePos; // pos2?
	Vector3 _fb0;
	Vector3 m_vHitNormal;
	void* m_pReference;
	void* _fd4;
	uint32_t m_dwHitMaterialId;
	uint32_t _fdc;
	float _fe0;	// 1.0 is default
	float _fe4;	// 1.0 is default
	uint8_t __e8[0x8];
	float _ff0;						//                           . 0                      
	float _ff4;						//                           . 0                      
	float _ff8;						//                           . 0                      
	uint32_t _ffc;					// 0xcdcdcdcd
	float _f100;
	float _f104;
	float _f108;					//                           . 0                      
	float _f10c;					//                           . 0                      
	Vector3 m_vGroundVelocity;					// w - always -1?
	Vector3 m_vTyreContactVelocity;
	float m_fSkidmarkIntensity;					//                      |velocity? if == 0 then no tyre track on floor
	float _f134;					// m_fTyreStabilityCoefficient?
	float m_fPhysicsOffsetLeftRight;					//                            lf
	float m_fRotationZ;				// 13C-140 rotation in z-axis.
	float m_fBrakeForce;
	float m_fDriveForce;
	float _f148;
	float m_fRotationZOld;			//          ?
	float m_fBrakeForceOld;
	float m_fDriveForceOld;
	float m_fFrictionDamage; // maybe
	float m_fHealth;				// 15C-160
	float m_fTyreHealth;			// 160-164
	union {
		struct {
			uint32_t m_bSpringsAreActive : 1;
			uint32_t m_bTouchesGround : 1;
			uint32_t m_bCurrentIsDrive : 1; // if throttle > 0.f?
			uint32_t m_bWheelIsOnFire : 1;
			uint32_t flag4 : 1; // without track?
			uint32_t flag5 : 1; // without track?
			uint32_t flag6 : 1;
			uint32_t flag7 : 1;

			uint32_t flag8 : 1;
			uint32_t flag9 : 1;
			uint32_t flagA : 1; //          ?
			uint32_t flagB : 1; // used with fragInstGta from CVehicle | is left or right wheel? mirror?
			uint32_t m_bIsRearWheel : 1; //                1                                                   
			uint32_t m_bFrontWheelSteer : 1;
			uint32_t m_bRearWheelSteer : 1;
			uint32_t m_bDriveWheel : 1;

			uint32_t m_bSuspensionMovementOfTheRightWheel : 1;
			uint32_t m_bSuspensionMovementOfTheLeftWheel : 1;
			uint32_t flag12 : 1; // low gravity? 1 if r have separate model
			uint32_t flag13 : 1; // 1 if r have separate model
			uint32_t flag14 : 1;
			uint32_t flag15 : 1;
			uint32_t m_bMirrorWheel : 1; // mirror wheel model
			uint32_t flag17 : 1;

			uint32_t m_bScaleWheelByCollision : 1;	// for example, the rear wheel airtug is larger than the front
			uint32_t flag19 : 1; // stranniy shum pri ezde. Voda?
			uint32_t m_bRaiseSuspension1 : 1;
			uint32_t m_bRaiseSuspension2 : 1;
			uint32_t m_bBurnout : 1; // on idle
			uint32_t flag1D : 1;
			uint32_t flag1E : 1;
			uint32_t flag1F : 1;
		};
		uint32_t m_dwFlags;
	};
	float _f168; // if _f168 < 0 then _f168 = 
	float _f16c; // if m_flags.bWheelIsOnFire then _f16c = 0



};