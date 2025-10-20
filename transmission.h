#pragma once

#include "helpers.h"


struct CEntity;

struct __declspec(align(4)) CTransmission {

	enum eTransFlags : uint16_t {
		CHANGE_UP_TIME = 0x1,
		CHANGE_DOWN_TIME = 0x2,
		THROTTLE_ENGAGED = 0x4,
	};

	static float ms_fIdleRpm;
	static float ms_fMaxClutchRatio;
	static float ms_fMinClutchRatio;
	static float ms_fTransChangeClutch;
	static float ms_fStartingRpm;
	static float ms_fStartingClutchRatio;
	static int32_t ms_nStartingGear;

	short m_sGear;
	uint16_t m_wFlags;
	float m_fRevs;
	float m_fRevsOld;
	uint32_t _fC;
	float m_fClutchRatio;
	float m_fThrottle;
	int32_t m_nGearChangeTime;
	float m_fEngineHealth;
	float m_fFireFactor;
	CEntity* m_pEntityThatSetUsOnFire;

	float processEngine(CVehicle* parentVehicle, int32_t numberOfDriveWheels, float wheelSpeed, float vehicleSpeed, float timeStep);
	void processGears(CVehicle* parent, int32_t wheelCount, uint32_t numOfWorkingSus, float fSpeedFromWheels, float fSpeedFromVehicle, float fTimeStep);
	void processStarting(float timestep);
	void processOff(float timestep);
	float process(CVehicle* parent, float* pWheelsSpeed, int32_t numWheels, int32_t numWheelsOnGround, float timestep);

	static void patch();
};

