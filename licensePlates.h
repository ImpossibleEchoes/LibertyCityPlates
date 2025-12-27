#pragma once
#include "vehExtParams.h"

#include <inttypes.h>
#include <vector>

struct CCustomShaderEffectVehicleFX;
struct CVehicle;

struct CPlateFactory {
	static constexpr uint8_t MAX_NUM_PLATES = 254;
	static constexpr uint8_t MAX_NUM_PLATES_TEXTURES = 254;

	static constexpr uint8_t PLATE_CHAR_RANDOM = 0xFF;
	static constexpr uint8_t PLATE_NUM_RANDOM = 0xFE;
	static constexpr uint8_t PLATE_FULL_RANDOM = 0xFD;
	static constexpr uint8_t PLATE_LOWERCASE_AB_RANDOM = 0xFC;

	static std::map<uint32_t, std::vector<uint8_t>> ms_vehPlates;

	static __forceinline void clearUsedPlates() { ms_vehPlates.clear(); }

	struct PlateTxds {

		static const char* ms_pszDictName;
		static const char* ms_DictPath;

		static uint32_t ms_LibertyCityPlatesTxd;

		// orig call
		static size_t ms_load;
		static size_t ms_release;

		static void* load();
		static void* release();

		static void init();
	};

	static struct PlateTxdsTable {
		struct Textures {
			uint32_t m_Plate_d;
			uint32_t m_Plate_n;
			uint32_t m_Plate_s;
			uint32_t m_Letters_d;
			uint32_t m_Letters_n;
		};

		Textures m_aTextures[MAX_NUM_PLATES_TEXTURES];

		void init();
	} ms_plateTexturesHashes;

	static struct alignas(0x10) PlateInfo {
		Vector4 m_vecLettersOffset; // ToDo: Vector2
		Vector4 m_vecLettersScale;
		Vector4 m_vecLettersTint;

		uint8_t m_aFormat[0xC];
		uint8_t m_NumLetters;
		uint8_t m_LettersTxdId;
		uint8_t m_PlateTxdId;

		std::vector<Rect> m_rects;
		uint8_t m_ParentId;
		uint8_t m_Chance; // if m_pParent == nullptr then m_Chance always = 100


		PlateInfo() {
			//m_PlateTxd = nullptr;
			//m_LettersTxd = nullptr;
			m_NumLetters = 0;
			m_LettersTxdId = 0;
			m_PlateTxdId = 0;
			m_ParentId = ~0;
		}
	} ms_plates[MAX_NUM_PLATES];


	// read
	static void parseCarPlatesIde(char* line);
	static void parseLicensePlatesIde(char* line);

	// export 
	static void exportCarPlatesIde(FILE* f);
	static void exportLicensePlatesIde(FILE* f);

	// shaderFX
	static void __cdecl updateLicensePlates(CCustomShaderEffectVehicleFX*, CVehicle* pVeh, CVehExtParams::Params* pExtParams, CCustomShaderEffectVehicleFX::CAddonVars* pVars);
	static void __cdecl setVarsLicensePlates(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars);
	static void __cdecl initLicensePlates(CCustomShaderEffectVehicleFX* pFx, CCustomShaderEffectVehicleFX::CAddonVars* pVars);

	// params
	static void setPlateParams(CVehicle* pVeh, CVehExtParams::Params* pParams, int index);
	static void __cdecl setStandartLicensePlate(CVehicle* pVeh, CVehExtParams::Params* pParams);


	/* ============================== */

	static void init();

};


int32_t __cdecl setCarLicensePlateText0123(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3);
int32_t __cdecl setCarLicensePlateText4567(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3);
int32_t __cdecl setCarLicensePlateText891011(int32_t vehHandle, uint8_t char0, uint8_t char1, uint8_t char2, uint8_t char3);

int32_t __cdecl setCarLicensePlateTextTintUByte4(int32_t vehHandle, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
int32_t __cdecl setCarLicensePlateTextTintFloat4(int32_t vehHandle, float r, float g, float b, float a);
int32_t __cdecl setCarLicensePlateTextLength(int32_t vehHandle, uint8_t length);
int32_t __cdecl setCarLicensePlateTextTexture(int32_t vehHandle, uint8_t texture);
int32_t __cdecl setCarLicensePlatePlateTexture(int32_t vehHandle, uint8_t texture);
int32_t __cdecl setCarLicensePlateTextScale(int32_t vehHandle, float x, float y);
int32_t __cdecl setCarLicensePlateTextOffset(int32_t vehHandle, float x, float y);