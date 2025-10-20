#pragma once
#include <map>

enum eNewVehFlags {
	NEWVEHFLAG_STEER_FR_WHEELS = 1,
	NEWVEHFLAG_SKIDMARK_ONLY_FROM_LAST_WHEEL_BY_VECTOR = 1 << 1,
};

	struct CVehicleModelInfo;

struct CVehNewFlags {
	struct Flags {
		uint32_t m_flags;

	};

	static std::map<CVehicleModelInfo*, Flags> ms_flags;

	static __forceinline Flags* getNewVehFlags(CVehicleModelInfo* pVehMdl) {
		if (pVehMdl && ms_flags.count(pVehMdl))
			return &ms_flags[pVehMdl];
		return nullptr;
	}

	static void init();

	void* runHook(const char* flags);
	static size_t ms_readFlags_origcall;
};