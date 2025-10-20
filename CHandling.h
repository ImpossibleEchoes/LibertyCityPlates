#pragma once
#include <inttypes.h>

enum eModelFlags : uint32_t {
	// Типи транспортних засобів
	MODELFLAG_IS_VAN = (1 << 0),  // bIsVan
	MODELFLAG_IS_BUS = (1 << 1),  // bIsBus
	MODELFLAG_IS_LOW = (1 << 2),  // bIsLow
	MODELFLAG_IS_BIG = (1 << 3),  // bIsBig

	// Прапори ABS
	MODELFLAG_ABS_STD = (1 << 4),  // bAbsStd
	MODELFLAG_ABS_OPTION = (1 << 5),  // bAbsOption
	MODELFLAG_ABS_ALT_STD = (1 << 6),  // bAbsAltStd
	MODELFLAG_ABS_ALT_OPTION = (1 << 7),  // bAbsAltOption

	// Прапори дверей та сидінь
	MODELFLAG_NO_DOORS = (1 << 8),  // bNoDoors
	MODELFLAG_TANDEM_SEATS = (1 << 9),  // bTandemSeats
	MODELFLAG_SIT_IN_BOAT = (1 << 10), // bSitInBoat
	MODELFLAG_NONE = (1 << 11), // bNone

	// Прапори вихлопу
	MODELFLAG_NO_EXHAUST = (1 << 12), // bNoExhaust
	MODELFLAG_DOUBLE_EXHAUST = (1 << 13), // bDoubleExhaust

	// Прапори камери та входу
	MODELFLAG_NO_1FPS_LOOK_BEHIND = (1 << 14), // bNo1fpsLookBehind
	MODELFLAG_CAN_ENTER_IF_NO_DOOR = (1 << 15), // bCanEnterIfNoDoor

	// Прапори передньої осі
	MODELFLAG_AXLE_F_NOTILT = (1 << 16), // bAxleFNotilt
	MODELFLAG_AXLE_F_SOLID = (1 << 17), // bAxleFSolid
	MODELFLAG_AXLE_F_MCPHERSON = (1 << 18), // bAxleFMcpherson
	MODELFLAG_AXLE_F_REVERSE = (1 << 19), // bAxleFReverse

	// Прапори задньої осі
	MODELFLAG_AXLE_R_NOTILT = (1 << 20), // bAxleRNotilt
	MODELFLAG_AXLE_R_SOLID = (1 << 21), // bAxleRSolid
	MODELFLAG_AXLE_R_MCPHERSON = (1 << 22), // bAxleRMcpherson
	MODELFLAG_AXLE_R_REVERSE = (1 << 23), // bAxleRReverse

	// Прапори категорій транспортних засобів
	MODELFLAG_IS_BIKE = (1 << 24), // bIsBike
	MODELFLAG_IS_HELI = (1 << 25), // bIsHeli
	MODELFLAG_IS_PLANE = (1 << 26), // bIsPlane
	MODELFLAG_IS_BOAT = (1 << 27), // bIsBoat

	// Інші прапори
	MODELFLAG_BOUNCE_PANELS = (1 << 28), // bBouncePanels
	MODELFLAG_DOUBLE_R_WHEELS = (1 << 29), // bDoubleRWheels
	MODELFLAG_FORCE_GROUND_CLEARANCE = (1 << 30), // bForceGroundClearance
	MODELFLAG_IS_HATCHBACK = (1UL << 31) // bIsHatchback
};

enum class HandlingFlags : uint32_t {
	HANDLINGFLAG_SMOOTH_COMPRESSION = (1 << 0),  // bSmoothCompresn
	HANDLINGFLAG_NONE_1 = (1 << 1),  // bNone
	HANDLINGFLAG_NPC_ANTI_ROLL = (1 << 2),  // bNpcAntiRoll
	HANDLINGFLAG_NPC_NEUTRAL_HANDLING = (1 << 3),  // bNpcNeutralHandl
	HANDLINGFLAG_NO_HANDBRAKE = (1 << 4),  // bNoHandbrake
	HANDLINGFLAG_STEER_REARWHEELS = (1 << 5),  // bSteerRearwheels
	HANDLINGFLAG_HB_REARWHEEL_STEER = (1 << 6),  // bHbRearwheelSteer
	HANDLINGFLAG_ALT_STEER_OPTION = (1 << 7),  // bAltSteerOpt

	// Ширина передніх коліс
	HANDLINGFLAG_WHEEL_F_NARROW2 = (1 << 8),  // bWheelFNarrow2
	HANDLINGFLAG_WHEEL_F_NARROW = (1 << 9),  // bWheelFNarrow
	HANDLINGFLAG_WHEEL_F_WIDE = (1 << 10), // bWheelFWide
	HANDLINGFLAG_WHEEL_F_WIDE2 = (1 << 11), // bWheelFWide2

	// Ширина задніх коліс
	HANDLINGFLAG_WHEEL_R_NARROW2 = (1 << 12), // bWheelRNarrow2
	HANDLINGFLAG_WHEEL_R_NARROW = (1 << 13), // bWheelRNarrow
	HANDLINGFLAG_WHEEL_R_WIDE = (1 << 14), // bWheelRWide
	HANDLINGFLAG_WHEEL_R_WIDE2 = (1 << 15), // bWheelRWide2

	// Гідравліка
	HANDLINGFLAG_HYDRAULIC_GEOMETRY = (1 << 16), // bHydraulicGeom
	HANDLINGFLAG_HYDRAULIC_INSTALLED = (1 << 17), // bHydraulicInst
	HANDLINGFLAG_HYDRAULIC_NONE = (1 << 18), // bHydraulicNone

	HANDLINGFLAG_NOS_INSTALLED = (1 << 19), // bNosInst

	// Прохідність по бездоріжжю
	HANDLINGFLAG_OFFROAD_ABILITY = (1 << 20), // bOffroadAbility
	HANDLINGFLAG_OFFROAD_ABILITY2 = (1 << 21), // bOffroadAbility2

	HANDLINGFLAG_HALOGEN_LIGHTS = (1 << 22), // bHalogenLights
	HANDLINGFLAG_PROC_REARWHEEL_1ST = (1 << 23), // bProcRearwheel1st
	HANDLINGFLAG_USE_MAX_SPEED_LIMIT = (1 << 24), // bUseMaxspLimit
	HANDLINGFLAG_LOW_RIDER = (1 << 25), // bLowRider
	HANDLINGFLAG_STREET_RACER = (1 << 26), // bStreetRacer
	HANDLINGFLAG_NONE_2 = (1 << 27), // bNone2
	HANDLINGFLAG_SWINGING_CHASSIS = (1 << 28), // bSwingingChassis
	HANDLINGFLAG_OLD_PHYSICS = (1 << 29), // bOldPhysics
	HANDLINGFLAG_NONE_3 = (1 << 30), // bNone3
	HANDLINGFLAG_NONE_4 = (1ULL << 31) // bNone4 (використовуємо 1ULL для 31-го біта, якщо unsigned int 32-бітний)
};

struct CHandlingVehicle {
	char m_pszId[16];
	float m_fMass;
	float m_fDragMult;
	uint32_t field_18;
	uint32_t field_1C;
	float m_centreOfMass[3];
	uint32_t field_2C;
	int32_t m_nPercentSubmerged;
	uint32_t field_34;
	float m_fDriveFront;
	float m_fDriveRear;
	int32_t m_nDriveGears;
	float m_fDriveForce;
	float m_fDriveInertia;
	float m_fV_times12;
	float m_fV;
	float m_fV_gearR;
	float m_fV_gear1;
	float m_fV_gear2;
	float m_fV_gear3;
	float m_fV_gear4;
	float m_fV_gear5;
	float m_fV_gear6;
	float m_fV_gear7;
	float m_fBrakeForce;
	float m_fBrakeFront;
	float m_fBrakeRear;
	float m_fSteeringLock;
	float m_fTractionCurveMax;
	uint32_t field_88;
	float m_fTractionCurveMin;
	uint32_t field_90;
	float m_fTractionCurveLateral;
	uint32_t field_98;
	float m_fTractionCurveLongitudinal;
	uint32_t field_A0;
	float m_fTractionSpringDeltaMax;
	uint32_t field_A8;
	float m_fTractionFront;
	float m_fTractionRear;
	float m_fSuspensionForce;
	float m_fSuspensionCompDamp;
	float m_fSuspensionReboundDamp;
	float m_fSuspensionUpperLimit;
	float m_fSuspensionLowerLimit;
	float m_fSuspensionRaise;
	float m_fSuspensionFront;
	float m_fSuspensionRear;
	float m_fCollisionDamageMult;
	float m_fWeaponDamageMult;
	float m_fDeformationDamageMult;
	float m_fEngineDamageMult;
	float m_fSeatOffsetDist;
	float m_nMonetaryValue;
	uint32_t m_dwModelFlags;
	uint32_t m_dwHandlingFlags;
	uint8_t field_F4;
	uint8_t field_F5[3];
	void* m_pBikeHandling;
	void* m_pFlyingHandling;
	void* m_pBoatHandling;
	uint32_t field_104;
	uint32_t field_108;
	uint32_t field_10C;

	__forceinline float getGearRatio(int32_t gear) const { return *(&m_fV_gearR + gear); }
};