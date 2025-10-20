// dllmain.cpp : Defines the entry point for the DLL application.

#include <stdio.h>
#include "hookFns.h"
#include "addrs.h"
#include "vehExtParams.h"
#include <Windows.h>
#include <cstdlib>

#include "config.h"
#include "newComponents.h"
#include "helpers.h"
#include "transmission.h"
#include "bumpers.h"
#include "double_skidmark_fix.h"
#include "tankComponents.h"
#include "newVehFlags.h"
#include "weapon.h"

static void enableConsole() {
	FILE* pFile = NULL;
	AllocConsole();
	SetConsoleTitleA("cringe");
	freopen_s(&pFile, "CONOUT$", "w", stdout);
	freopen_s(&pFile, "CONOUT$", "w", stderr);
	freopen_s(&pFile, "CONIN$", "r", stdin);
}

void runTest(); // from testZone.cpp

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {

#ifdef _DEBUG
		enableConsole();
		printDebug("%s\n", __FUNCSIG__);
		printDebug("%u\n", g_baseAddress);
		system("pause");
#endif // _DEBUG

		if constexpr(0)
			initAddrsEFLC1120();
		else {
			DWORD preCEresult = initAddrsDynamicLegacy();
			if (preCEresult) {
				DWORD CEresult = initAddrsDynamicCE();
				if (CEresult) {
					char msg[0xff];
					sprintf_s(msg, sizeof msg, "Addresses could not be determined.\nMake sure you are using the latest version of Grand Theft Auto IV: Complete Edition");
					MessageBoxA(nullptr, msg, nullptr, 0x10);
					break;
				}
				g_bIsCE = true;

			}
		}
		initHelpers();

		CConfig::read();
	
		//g_sound.init(nullptr);

		//runTest();

		CVehExtParams::init();
		CCustomShaderEffectVehicleFX::initHooks();
		CVehicleFactory::init();
		CTransmission::patch();
		initBumpers();
		doubleSidemarkFix::init();
		CVehNewFlags::init();

		vehicleScript::init();

		initNewComponents();
		initTankComponents();
		initWeapons();

		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

