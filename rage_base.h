#pragma once
#include <inttypes.h>

template<typename _Type> struct atArray {
	_Type* m_pElements;
	uint16_t m_Count;
	uint16_t m_Size;
};

template<typename _Type> struct pgDictionary {
	size_t __vmt;
	size_t m_map;
	pgDictionary<_Type>* m_pParent;
	uint32_t m_RefCount;
	atArray<uint32_t> m_Hashes;
	atArray<_Type*> m_Entries;

	_Type* __thiscall lookup(uint32_t hash) {
		int left = 0;
		int right = m_Hashes.m_Count - 1;
		if (right >= 0) {
			while (true) {
				auto mid = (right + left) >> 1;
				if (hash == m_Hashes.m_pElements[mid])
					return m_Entries.m_pElements[mid];
				if (hash >= m_Hashes.m_pElements[mid])
					left = mid + 1;
				else
					right = mid - 1;
				if (left > right) {
					if (m_pParent)
						return m_pParent->lookup(hash);
					else
						return nullptr;
				}
			}
		}
	}
};