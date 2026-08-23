#pragma once

struct CConfig {
public:
	static bool ms_bLicensePlates;
	static bool ms_bDashboard;
	static bool ms_bPatchCustomVehFX;
	static bool ms_bPatchEngine;
	static bool ms_bEngineComponents;
	static float ms_fBumperStrength;
	static bool ms_bBumperStrengthIsMassMultiplier;
	static float ms_fMinBumperStrength;
	static bool ms_bFixDoubleSkidmark;
	static bool ms_bPopupHeadlights;
	static bool ms_bTankComponents;
	static bool ms_bMoreWheels;
	static bool ms_bImportExportData;
	static bool ms_bUseFastRandom;
	static bool ms_bPlateLight;

	static float ms_fPlateLightColorRed;
	static float ms_fPlateLightColorGreen;
	static float ms_fPlateLightColorBlue;
	static float ms_fPlateLightIntensity;
	static float ms_fPlateLightRadius;
	static float ms_fPlateLightInnerConeAngle;
	static float ms_fPlateLightOuterConeAngle;
	static float ms_fPlateLightPitch;
	static float ms_fPlateLightHeightOffset;

	static float ms_fPlateBumpiness;

	static bool ms_bTrue;

	static void read();
	static bool create();
};