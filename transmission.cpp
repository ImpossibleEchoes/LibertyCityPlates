#include "transmission.h"

#include "CHandling.h"
#include "rage_math.h"
#include "addrs.h"
#include "utils.h"
#include "config.h"
#include "hookFns.h"
#include "trace.h"
#include "vehExtParams.h"
#include "CVehicle.h"

void __cdecl updateEngineVars(CCustomShaderEffectVehicleFX*, CVehicle*, CVehExtParams::Params* pExtParams,
	CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	if (pVars->m_nTrack2Index > 0)
		pVars->m_fTrack2 = (pExtParams->getEngineRot() + RAGE_PI) * (1 / (RAGE_PI * 2));
}

void __cdecl setVarsEngineVars(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	if (pVars->m_nTrack2Index)
		rage__grmShaderGroup__setVarVector4(pFx->m_pShaderGroup, pVars->m_nTrack2Index, &pVars->m_fTrack2);
}

void __cdecl initEngineVars(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars) {
	pVars->m_nTrack2Index = rage__grmShaderGroup__addShaderGroupVar(pFx->m_pShaderGroup, "uvtrack2", false);
}

float CTransmission::ms_fIdleRpm = 0.1f;
float CTransmission::ms_fMaxClutchRatio = 1.f;
float CTransmission::ms_fMinClutchRatio = 0.f;
float CTransmission::ms_fTransChangeClutch = 0.1f;
float CTransmission::ms_fStartingRpm = 0.05f;
float CTransmission::ms_fStartingClutchRatio = 0.f;
int32_t CTransmission::ms_nStartingGear = 1;

void CTransmission::patch() {
	if (CConfig::ms_bPatchEngine) {
		setFnAddrInCallOpcode(g_hookAddr_CTransmission__process, getThisCallAddr(&process));
		PRINT_DUBUG("[CTransmission::patch(...)] CTransmission::process is patched\n");

		if (CConfig::ms_bPatchCustomVehFX) {
			g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnInit = initEngineVars;
			g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnSetVars = setVarsEngineVars;
			g_customVehFxAddons[g_numCustomVehFxAddons].m_pfnUpdate = updateEngineVars;
			g_numCustomVehFxAddons++;
			PRINT_DUBUG("[CTransmission::patch(...)] added uvtrack2\n");

		}
		writeDWORD(g_CAutomobile__processPhysics_engineOnCheckJump, 0);
		PRINT_DUBUG("[CTransmission::patch(...)] engineOn check pached\n");

	}
	
}

void CTransmission::processStarting(float timestep) {
	if (m_fRevs < ms_fStartingRpm)
		m_fRevs = smoothFloat2(ms_fStartingRpm, m_fRevs, timestep, 2);
	else if (m_fRevs > ms_fStartingRpm)
		m_fRevs = smoothFloat2(ms_fStartingRpm, m_fRevs, timestep, 1);

	if (m_fClutchRatio > ms_fStartingClutchRatio)
		m_fClutchRatio = smoothFloat(ms_fStartingClutchRatio, m_fClutchRatio, 2.f, timestep);

	if (m_sGear != ms_nStartingGear)
		m_sGear = ms_nStartingGear;

	if (m_fThrottle)
		m_fThrottle = 0.f;

}

void CTransmission::processOff(float timestep) {
	if (m_fRevs > 0.f)
		m_fRevs = smoothFloat2(0.f, m_fRevs, timestep, 2.f);

	if (m_fClutchRatio < 1.f)
		m_fClutchRatio = smoothFloat(1.f, m_fClutchRatio, 2.f, timestep);

	if (m_sGear != ms_nStartingGear)
		m_sGear = ms_nStartingGear;

	if (m_fThrottle)
		m_fThrottle = 0.f;
}

float CTransmission::process(CVehicle* parent, float* pWheelsSpeed, int32_t numWheels, int32_t numWheelsOnGround, float timestep) {
	m_fRevsOld = m_fRevs;
	char vehicleFlags1_0 = g_pfnGetVehFlags1_0(parent);

	if ((vehicleFlags1_0 & 8) != 0) { // on
		float totalSpeed = 0.f;

		for (int i = 0; i < numWheels; ++i)
			totalSpeed += pWheelsSpeed[i];

		float fSpeedFromVehicle;
		float fSpeedFromWheels;

		if (numWheels > 0) // Unique division by zero
			fSpeedFromWheels = totalSpeed / numWheels;
		else
			fSpeedFromWheels = 0.f;

		Matrix34* pCoords = parent->getTransform();
		Vector3 rightVec;
		((Vector3 * (__thiscall*)(CVehicle*, Vector3*))(*(void***)parent)[236 / 4])(parent, &rightVec);
		fSpeedFromVehicle = rightVec.z * pCoords->b.z + rightVec.y * pCoords->b.y + rightVec.x * pCoords->b.x;
		bool isSpeedValid = true;
		if (fSpeedFromWheels <= 0.0)
			isSpeedValid = fSpeedFromVehicle <= 0.0;
		else
			isSpeedValid = fSpeedFromVehicle >= 0.0;
		if (!isSpeedValid)
			fSpeedFromVehicle = 0.0;
		processGears(parent, numWheels, numWheelsOnGround, fSpeedFromWheels, fSpeedFromVehicle, timestep);
		return processEngine(parent, numWheels, fSpeedFromWheels, fSpeedFromVehicle, timestep);
	}
	else if ((vehicleFlags1_0 & 0x10) != 0) // Starting
		processStarting(timestep);
	else // off
		processOff(timestep);

	return 0.f;

}

void CTransmission::processGears(CVehicle* parent, int32_t wheelCount, uint32_t numOfWorkingSus, float fSpeedFromWheels, float fSpeedFromVehicle, float fTimeStep) {

	float m_fGasPedal = g_pfnGetVehGasPedal(parent);
	float m_fBrakePedal = g_pfnGetVehBrakePedal(parent);

	CHandlingVehicle* m_pHandling = g_pfnGetHandlingData(parent);

	uint32_t _dwGameTimer = *g_pdwGameTimer;

	float absGasPedal = m_fGasPedal < 0.f ? -m_fGasPedal : m_fGasPedal;
	float absBrakePedal = m_fBrakePedal < 0.f ? -m_fBrakePedal : m_fBrakePedal;

	float combinedPedalInput = (absBrakePedal * 0.9f) + absGasPedal;
	if (combinedPedalInput > 1.f)
		combinedPedalInput = 1.f;
	float adjustedPedalInput = (combinedPedalInput * 0.3f) + 0.7f;

	if (absGasPedal < 0.7f)
		m_wFlags &= ~THROTTLE_ENGAGED; // Reset THROTTLE_ENGAGED

	uint32_t currentFlags = m_wFlags;
	bool canShiftUp = false;
	if ((currentFlags & CHANGE_DOWN_TIME) == 0 || (currentFlags & THROTTLE_ENGAGED) == 0 || (canShiftUp = false, (_dwGameTimer >= m_nGearChangeTime + 2000)))
		canShiftUp = true;
	else
		canShiftUp = false;

	bool canShiftDown = true;
	if ((currentFlags & CHANGE_UP_TIME) != 0 && (currentFlags & THROTTLE_ENGAGED) != 0) {
		uint32_t end = m_nGearChangeTime + 3000;
		if (_dwGameTimer < end)
			canShiftDown = 0;
	}

	float speedToUse1 = fSpeedFromWheels <= fSpeedFromVehicle ? fSpeedFromWheels : fSpeedFromVehicle; // ToDo: min(fSpeedFromWheels, fSpeedFromVehicle)
	float speedToUse2 = fSpeedFromWheels >= fSpeedFromVehicle ? fSpeedFromWheels : fSpeedFromVehicle; // ToDo: max(fSpeedFromWheels, fSpeedFromVehicle)

	// Process the clutch if it is less than the maximum
	if (m_fClutchRatio < ms_fMaxClutchRatio) {
		float clutchIncreaseFactor = m_pHandling->m_fDriveInertia >= 1.0f ? 1.3f : 0.6f;

		m_fClutchRatio += (clutchIncreaseFactor * fTimeStep);
		if (m_fClutchRatio > ms_fMaxClutchRatio)
			m_fClutchRatio = ms_fMaxClutchRatio;
	}

	// Engage first gear(if the gas pedal is not pressed)
	if (absGasPedal < 0.01f && (m_pHandling->m_fV_gear1 / m_pHandling->m_fV_times12) * fSpeedFromWheels < 0.2f) {
		m_sGear = 1;
		m_fClutchRatio = ms_fTransChangeClutch;
		return;
	}

	// Engage reverse gear
	if (m_fGasPedal < 0.f && (m_pHandling->m_fV_gearR / m_pHandling->m_fV_times12) * speedToUse1 > -0.5f) {
		//if (m_sGear == 0) { // Если не на задней
		if (m_sGear != 0) { // Если не на задней
			m_sGear = 0; // Включить заднюю
			m_fClutchRatio = ms_fTransChangeClutch;
		}
		return;
	}

	// Reverse gear to first gear
	if (m_fGasPedal > 0.f && m_sGear == 0 && (m_pHandling->m_fV_gear1 / m_pHandling->m_fV_times12) * speedToUse1 < 0.5f) {
		m_sGear = 1;
		m_fClutchRatio = ms_fTransChangeClutch;
		return;
	}

	float gearRatio = m_pHandling->getGearRatio(m_sGear);
	float gearSpeed = (gearRatio / m_pHandling->m_fV_times12) * speedToUse1;


	if (gearSpeed <= (adjustedPedalInput * 0.93000001f)) {
		// Downshift
		if (m_sGear > 1 &&
			(adjustedPedalInput * 0.75f) > (m_pHandling->getGearRatio(m_sGear - 1) / m_pHandling->m_fV_times12) * speedToUse2 && canShiftDown
			) {
			//if (m_sGear > 2) {

				m_sGear--;
				m_fClutchRatio = ms_fTransChangeClutch;
				m_wFlags = CHANGE_DOWN_TIME | THROTTLE_ENGAGED;
				m_nGearChangeTime = _dwGameTimer;
			//}
		}
	}
	else if (m_sGear < *reinterpret_cast<uint8_t*>(&m_pHandling->m_nDriveGears) && m_fGasPedal > 0.0f) {
		// Upshift if the drive wheels touch the ground
		if (numOfWorkingSus == wheelCount) {
			if (canShiftUp) {
				m_sGear++;
				m_fClutchRatio = ms_fTransChangeClutch;
				m_wFlags = CHANGE_UP_TIME | THROTTLE_ENGAGED;
				m_nGearChangeTime = _dwGameTimer;
			}
		}
	}
}

float CTransmission::processEngine(CVehicle* parentVehicle, int32_t numberOfDriveWheels, float wheelSpeed, float vehicleSpeed, float timeStep) {
	float driveForce = 0.f;

	Vector3 tmpVec;

	// Getting data from the parent
	char vehicleFlags1_0 = g_pfnGetVehFlags1_0(parentVehicle);
	float gasPedal = g_pfnGetVehGasPedal(parentVehicle);
	CHandlingVehicle* handlingData = g_pfnGetHandlingData(parentVehicle);
	uint32_t specialVehicleType = g_pfnGetVehSpecialType(parentVehicle);
	uint32_t vehicleField_FE0 = g_pfnGetVeh_FE0(parentVehicle);

	m_fThrottle = gasPedal;


	if ((gasPedal < 0.f && m_sGear > 0) || (gasPedal > 0.f && !m_sGear)) {
		m_fThrottle = 0.f;
		return 0.f;
	}

	// If the handbrake is applied, the clutch is below 0.6 and the gear is second or higher
	if (vehicleFlags1_0 >= 0 && m_fClutchRatio < 0.6f && m_sGear > 1)
		gasPedal = m_fThrottle = 0.f;

	short currentGear = m_sGear;
	float clutchRatio = m_fClutchRatio;

	// Calculating the desired engine speed based on wheel and gear speeds
	float calculatedEngineSpeed = (*(&handlingData->m_fV_gearR + m_sGear) / handlingData->m_fV_times12) * wheelSpeed;

	if (clutchRatio > ms_fMaxClutchRatio)
		clutchRatio = ms_fMaxClutchRatio;
	else if (clutchRatio < ms_fMinClutchRatio)
		clutchRatio = ms_fMinClutchRatio;
	float clutchEngagement = clutchRatio;

	// If the handbrake is on, then the clutch is 0
	if (vehicleFlags1_0 < 0)
		clutchEngagement = 0.f;

	float absoluteGasPedalInput = gasPedal < 0.f ? -gasPedal : gasPedal;

	// Calculation of engine speed taking into account clutch and throttle
	float targetEngineRPM = (1.0f - clutchEngagement) * absoluteGasPedalInput + clutchEngagement * calculatedEngineSpeed;

	// For bike(reverse)
	bool bIsBikeGearReverse;
	if (specialVehicleType == 1 && gasPedal < 0.f && !currentGear) {
		Vector3* vehicleForwardVector = &parentVehicle->getTransform()->b;
		Vector3* velocityVector = (Vector3*)(*(int32_t(__thiscall*)(CVehicle*, Vector3*))((*(size_t**)parentVehicle)[236 / 4]))(parentVehicle, &tmpVec);

		float forwardSpeed = (velocityVector->z * vehicleForwardVector->z) + (velocityVector->y * vehicleForwardVector->y) + (velocityVector->x * vehicleForwardVector->x);

		// 
		if (forwardSpeed > -2.f) {
			float adjustment = forwardSpeed - -2.f;
			if (adjustment > 1.f)
				adjustment = 1.f;
			driveForce = adjustment * -1.f;
		}
		bIsBikeGearReverse = true;
		targetEngineRPM = 0.f; // Even if he pushes the bike with his feet, the engine revs should not be 0. ToDo: fix it
		m_fClutchRatio = 0.f;
	}
	else
		bIsBikeGearReverse = false;


	if (ms_fIdleRpm > targetEngineRPM)
		targetEngineRPM = ms_fIdleRpm;

	// Update revs
	if (targetEngineRPM <= m_fRevs) {
		if (m_fRevs > targetEngineRPM) {
			m_fRevs -= (handlingData->m_fDriveInertia * timeStep);
			if (targetEngineRPM > m_fRevs)
				m_fRevs = targetEngineRPM;

		}
	}
	else {
		m_fRevs += (handlingData->m_fDriveInertia * timeStep);
		if (m_fRevs > targetEngineRPM)
			m_fRevs = targetEngineRPM;
	}

	// If it's not a bike or a bike that doesn't go backwards
	if (!bIsBikeGearReverse) {
		// Additional logic for revs and clutches
		if (m_fRevs < 1.f) { // We add a cutoff when accelerating on the handbrake
		//if (m_fRevs <= 1.f) { // Original check
			if (((*(&handlingData->m_fV_gearR + m_sGear) / handlingData->m_fV_times12) * vehicleSpeed) > 1.f)
				gasPedal = 0.f;
		}
		else {
			gasPedal *= 0.99f;
			m_fRevs = 0.9f;
			//m_fThrottle = 0.f;
		}

		if (m_sGear < 2) {
			float revsClutchAdjust = m_fRevs;
			if (revsClutchAdjust < 0.5f) {
				float clutchAdjustment = revsClutchAdjust * 2.0f;
				if (clutchAdjustment < 0.2f)
					clutchAdjustment = 0.2f;
				if (clutchAdjustment > m_fClutchRatio)
					clutchAdjustment = m_fClutchRatio;
				m_fClutchRatio = clutchAdjustment;
			}
		}

		float effectiveClutchRatio = clutchEngagement * 2.0f;
		if (effectiveClutchRatio > ms_fMaxClutchRatio)
			effectiveClutchRatio = ms_fMaxClutchRatio;

		float absoluteGasInputForTorque = gasPedal < 0.f ? -gasPedal : gasPedal;

		driveForce = (((*(&handlingData->m_fV_gearR + m_sGear) * handlingData->m_fDriveForce) * absoluteGasInputForTorque) * effectiveClutchRatio)
			* (m_fRevs / targetEngineRPM) / (float)numberOfDriveWheels;

		// Флаги? зачем? ToDo: explore it
		if ((vehicleField_FE0 & 0x800040) != 0)
			return driveForce * 1.5f;
		if ((vehicleField_FE0 & 0x400020) != 0)
			return driveForce * 1.2f;
	}
	return driveForce;
}
