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

	static bool ms_bTrue;

	static void read();
	static bool create();
};