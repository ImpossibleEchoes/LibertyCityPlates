#include "config.h"

#include <stdio.h>
#include <Windows.h>
#include <fstream>
#include <string>


#include "utils.h"

#define CONFIG_NAME "LibertyCityPlates.txt"
#define CONFIG_VERSION 1

bool CConfig::ms_bLicensePlates = false;
bool CConfig::ms_bDashboard = false;
bool CConfig::ms_bTrue = true;
bool CConfig::ms_bPatchCustomVehFX = false;
bool CConfig::ms_bPatchEngine = false;
bool CConfig::ms_bEngineComponents = false;
float CConfig::ms_fBumperStrength = -1.f;
bool CConfig::ms_bBumperStrengthIsMassMultiplier = true;
float CConfig::ms_fMinBumperStrength = 1000;
bool CConfig::ms_bFixDoubleSkidmark = false;
bool CConfig::ms_bPopupHeadlights = false;
bool CConfig::ms_bTankComponents = false;
bool CConfig::ms_bMoreWheels = false;
bool CConfig::ms_bImportExportData = false;
bool CConfig::ms_bUseFastRandom = false;
bool CConfig::ms_bPlateLight = false;

float CConfig::ms_fPlateLightColorRed = 1.0f;
float CConfig::ms_fPlateLightColorGreen = 1.0f;
float CConfig::ms_fPlateLightColorBlue = 1.0f;
float CConfig::ms_fPlateLightIntensity = 20.0f;
float CConfig::ms_fPlateLightRadius = 0.25f;
float CConfig::ms_fPlateLightInnerConeAngle = 90.0f;
float CConfig::ms_fPlateLightOuterConeAngle = 90.0f;
float CConfig::ms_fPlateLightPitch = 0.0f;
float CConfig::ms_fPlateLightHeightOffset = 0.0f;

void showError(const char* pszText) {
	MessageBoxA(nullptr, pszText, "LibertyCityPlates", 0x10 | MB_TOPMOST);
}

void showInfo(const char* pszText) {
	MessageBoxA(nullptr, pszText, "LibertyCityPlates", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
}

void showWarning(const char* pszText) {
	MessageBoxA(nullptr, pszText, "LibertyCityPlates", MB_OK | MB_ICONWARNING | MB_TOPMOST);
}

bool showQuestion(const char* pszText) {
	return MessageBoxA(nullptr, pszText, "LibertyCityPlates", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES;
}

bool CConfig::create() {
	FILE* f = fopen(CONFIG_NAME, "w");
	if (!f) {
		showError("unable to create config");
		return false;
	}

	fprintf(f, "version 1\n\n");
	fprintf(f, "LicensePlates 1\n");
	fprintf(f, "Dashboard 1\n");
	fprintf(f, "PatchCustomVehFX 1\n");
	fprintf(f, "PatchEngine 1\n");
	fprintf(f, "EngineComponents 1\n");
	fprintf(f, "Bumpers {\n");
	fprintf(f, "\tStrength 1.1\n");
	fprintf(f, "\tIsMassMultiplier 1\n");
	fprintf(f, "\tMinStrength 1000\n");
	fprintf(f, "}\n");
	fprintf(f, "FixDoubleSkidmark 1\n");
	//if(g_bIsCE)
		fprintf(f, "PopupHeadlights 1\n");
	//else
	//	fprintf(f, "PopupHeadlights 0\n");
	fprintf(f, "TankComponents 1\n");
	fprintf(f, "ImportExportData 0\n");
	fprintf(f, "UseFastRandom 0\n");
	fprintf(f, "PlateLight 1\n");
	fprintf(f, "PlateLightSettings {\n");
	fprintf(f, "\tColorRed 1.00\n");
	fprintf(f, "\tColorGreen 1.00\n");
	fprintf(f, "\tColorBlue 1.00\n");
	fprintf(f, "\tIntensity 20.00\n");
	fprintf(f, "\tRadius 0.25\n");
	fprintf(f, "\tInnerConeAngle 90.00\n");
	fprintf(f, "\tOuterConeAngle 90.00\n");
	fprintf(f, "\tPitch 0.00\n");
	fprintf(f, "\tHeightOffset 0.00\n");
	fprintf(f, "}\n");

	fclose(f);
	//showInfo("config created!");
	//ExitProcess(0);
	return true;

}

#define LICENSE_PLATES_HASH 0x3D7FB9A7
#define DASHBOARD_HASH 0x38B51187
#define PATCH_CUSTOM_VEH_FX_HASH 0x0B67D1DF
#define PATCH_ENGINE_HASH 0x13DB9A1C
#define ENGINE_COMPONENTS_HASH 0xFDC938CF
#define BUMPERS_HASH 0xDD06F037
#define STRENGHT_HASH 0xFF1C4627
#define IS_MASS_MULTIPLER_HASH 0x95312856
#define MIN_STRENGHT_HASH 0xA51F2E47
#define FIX_DOUBLE_SKIDMARK_HASH 0xE24D32BD
#define POPUP_HEADLIGHTS 0xA0DA9272
#define TANK_COMPONENTS 0xED7A5294
#define IMPORT_EXPORT_DATA 0x36E59D82
#define USE_FAST_RANDOM 0xB7F568BC
#define PLATE_LIGHT 0xC2A3B491
#define PLATE_LIGHT_SETTINGS_HASH 0xD03593B8
#define COLOR_RED_HASH 0x9F37D54F
#define COLOR_GREEN_HASH 0x1E7F9A71
#define COLOR_BLUE_HASH 0x8BA08810
#define INTENSITY_HASH 0xEFCD993D
#define RADIUS_HASH 0x4FBB9CF3
#define INNER_CONE_ANGLE_HASH 0xCE123CF2
#define OUTER_CONE_ANGLE_HASH 0x832BD195
#define PITCH_HASH 0x3F4BB8CC
#define HEIGHT_OFFSET_HASH 0x9B4A7D68

int getInt(std::fstream& f) {
	std::string tmpstr;
	f >> tmpstr;
	return std::stoi(tmpstr);
}
float getFloat(std::fstream& f) {
	std::string tmpstr;
	f >> tmpstr;
	return std::stof(tmpstr);
}

void CConfig::read() {
	std::fstream f(CONFIG_NAME);

	if (!f.is_open()) {
		if (create())
			read();
		return;
	}

	bool bError = false;

	std::string tmpstr;
	std::string tmpstr2;

	f >> tmpstr;
	f >> tmpstr2;
	if (tmpstr == "version" && std::stoi(tmpstr2) == CONFIG_VERSION) {
	//if (f.matchToken("version", 3) && f.getInt() == CONFIG_VERSION) {

		while (!f.eof()) {
			f >> tmpstr;
			if (f.eof())
				break;
			DWORD hash = jenkins_one_at_a_time_hash(tmpstr.c_str());
			switch (hash) {
			case LICENSE_PLATES_HASH:
				ms_bLicensePlates = getInt(f);
				break;
			case DASHBOARD_HASH:
				ms_bDashboard = getInt(f);
				break;
			case PATCH_CUSTOM_VEH_FX_HASH:
				ms_bPatchCustomVehFX = getInt(f);
				break;
			case PATCH_ENGINE_HASH:
				ms_bPatchEngine = getInt(f);
				break;
			case ENGINE_COMPONENTS_HASH:
				ms_bEngineComponents = getInt(f);
				break;
			case BUMPERS_HASH: {
				f >> tmpstr;
				if (tmpstr == "{") {
					while (true) {
						f >> tmpstr;
						if (tmpstr == "}")
							break;
						hash = jenkins_one_at_a_time_hash(tmpstr.c_str());

						switch (hash) {
						case STRENGHT_HASH:
							ms_fBumperStrength = getFloat(f);
							break;
						case IS_MASS_MULTIPLER_HASH:
							ms_bBumperStrengthIsMassMultiplier = getInt(f);
							break;
						case MIN_STRENGHT_HASH:
							ms_fMinBumperStrength = getFloat(f);
							break;
						}

					}
				}
				break;
			}
			case FIX_DOUBLE_SKIDMARK_HASH:
				ms_bFixDoubleSkidmark = getInt(f);
				break;

			case POPUP_HEADLIGHTS:
				ms_bPopupHeadlights = getInt(f);
				break;

			case TANK_COMPONENTS:
				ms_bTankComponents = getInt(f);
				break;

			case IMPORT_EXPORT_DATA:
				ms_bImportExportData = getInt(f);
				break;
			case USE_FAST_RANDOM:
				ms_bUseFastRandom = getInt(f);
				break;
			case PLATE_LIGHT:
				ms_bPlateLight = getInt(f);
				break;
			case PLATE_LIGHT_SETTINGS_HASH: {
				f >> tmpstr;
				if (tmpstr == "{") {
					while (true) {
						f >> tmpstr;
						if (tmpstr == "}")
							break;

						hash = jenkins_one_at_a_time_hash(tmpstr.c_str());

						switch (hash) {
						case COLOR_RED_HASH:
							ms_fPlateLightColorRed = getFloat(f);
							break;

						case COLOR_GREEN_HASH:
							ms_fPlateLightColorGreen = getFloat(f);
							break;

						case COLOR_BLUE_HASH:
							ms_fPlateLightColorBlue = getFloat(f);
							break;

						case INTENSITY_HASH:
							ms_fPlateLightIntensity = getFloat(f);
							break;

						case RADIUS_HASH:
							ms_fPlateLightRadius = getFloat(f);
							break;

						case INNER_CONE_ANGLE_HASH:
							ms_fPlateLightInnerConeAngle = getFloat(f);
							break;

						case OUTER_CONE_ANGLE_HASH:
							ms_fPlateLightOuterConeAngle = getFloat(f);
							break;

						case PITCH_HASH:
							ms_fPlateLightPitch = getFloat(f);
							break;

						case HEIGHT_OFFSET_HASH:
							ms_fPlateLightHeightOffset = getFloat(f);
							break;
						}
					}
				}
				break;
			}

			}
		}
	}
	else
		bError = true;

	f.close();

	if (bError) {
		if (create())
			read();
	}


}