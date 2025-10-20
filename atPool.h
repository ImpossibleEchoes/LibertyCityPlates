#pragma once

// by listener
template<typename T> class atPool {
public:
	uint8_t*	m_pObjects;
	uint8_t*	m_pFlags;
	uint32_t    m_dwSize;
	uint32_t    m_dwEntrySize;
	uint32_t    m_dwTop;
	uint32_t    m_dwUsed;
	uint8_t     m_bAllocated;
	uint8_t    _f19;
	uint8_t    _f1a;
	uint8_t    _f1b;

	atPool(uint32_t dwSize, const char* const pszName, uint32_t dwEntrySize = sizeof(T)) {
		// unlike original pool, atPool allocate only one memory chunk, for both m_pObjects and m_pFlags. 
		// m_pObjects must be 16-uint8_t aligned, so we should round up the pool size 
		m_dwSize = (dwSize + 15) & ~15;
		m_dwEntrySize = dwEntrySize > 16 ? (dwEntrySize + 15) & ~15 : (dwEntrySize + 3) & ~3;   // align objects in pool: large object is SSE-aligned, small object - uint32_t aligned
		m_pFlags = new uint8_t[m_dwSize * (m_dwEntrySize + 1)];  // 1 uint8_t for flag
		m_pObjects = m_pFlags + m_dwSize;
		m_bAllocated = true;
		m_dwTop = 0xffffffff;
		m_dwUsed = 0;
		memset(m_pFlags, 0x81, m_dwSize);  // POOL_ENTRY_FREE | (1 & POOL_GENERATION_MASK)
		//trace("[atPool::atPool] name = '%s', size = %d, entrySize = %d\n", pszName ? pszName : "*unknown*", m_dwSize, m_dwEntrySize);
	}

	void releaseAll() {
		if (m_dwSize > 0 && m_bAllocated && m_pFlags)
			delete m_pFlags;
		m_pFlags = m_pObjects = NULL;
		m_dwSize = 0;
		m_dwTop = 0xFFFFFFFF;
		m_dwUsed = 0;
		m_bAllocated = false;
	}

	__inline T* at(uint32_t dwIndex) { return m_pFlags[dwIndex] & 0x80 ? NULL : reinterpret_cast<T*> (m_pObjects + m_dwEntrySize * dwIndex); }  // TODO: range check ?
	__inline T* atHandle(uint32_t dwHandle) { return m_pFlags[dwHandle >> 8] & 0x80 ? NULL : reinterpret_cast<T*> (m_pObjects + m_dwEntrySize * (dwHandle>>8)); }  // TODO: range check ?
	__inline uint32_t indexOfHandle(uint32_t dwHandle) { return dwHandle>>8; }  // TODO: range check ?

	// internal allocate method. TODO: make private
	// scan 4 entry from dwIndex without checks. NOTE: one of this 4 entryes MUST be free;
	__inline T* allocateInternal(uint32_t dwIndex) {
		uint8_t* pf = m_pFlags + dwIndex;
		uint32_t j = 3;    // default - last entry
		if (pf[0] & 0x80) j = 0;
		else if (pf[1] & 0x80) j = 1;
		else if (pf[2] & 0x80) j = 2;

		dwIndex += j;
		pf += j;

		uint8_t f = ((*pf) + 1) & 0x7F;    // increment generation number, drop 'free' flag
		f += f == 0;    // generation number can't be zero
		*pf = f;
		m_dwTop = dwIndex;
		m_dwUsed++;
		return reinterpret_cast<T*>(m_pObjects + dwIndex * m_dwEntrySize);
	}

	T* allocate() { // E2> 4AAF70 (2224)
		uint32_t dwSplitIndex = (m_dwTop + 1) & ~3;  // align to uint32_t
		uint32_t* pdwFlags = reinterpret_cast<uint32_t*>(m_pFlags + dwSplitIndex);

		// scan from m_dwTop to end of the pool        
		for (uint32_t i = dwSplitIndex; i < m_dwSize; i += 4, pdwFlags++)
			if ((*pdwFlags) & 0x80808080)
				return allocateInternal(i);
		// scan from start of the pool to m_dwTop
		pdwFlags = reinterpret_cast<uint32_t*>(m_pFlags);
		for (uint32_t i = 0; i < dwSplitIndex; i += 4, pdwFlags++)
			if ((*pdwFlags) & 0x80808080)
				return allocateInternal(i);
		return NULL;
	}

	T* allocateAtHandle(uint32_t dwHandle) {
		uint32_t dwIndex = dwHandle >> 8;
		uint8_t f = dwHandle & 0x7F;   // generation number
		f += 0 == f;    // generation number can't be zero
		m_pFlags[dwIndex] = f;
		uint8_t* pf = m_pFlags;
		for (m_dwTop = 0; !(*pf & 0x80); pf++, m_dwTop++)
			;
		// TODO: assert m_dwTop < m_nSize
		m_dwUsed++;
		return reinterpret_cast<T*>(m_pObjects + dwIndex * m_dwEntrySize);
	}

	__inline void releaseByIndex(uint32_t dwIndex) {
		m_pFlags[dwIndex] |= 0x80;
		m_dwUsed--;
		if (dwIndex < m_dwTop)
			m_dwTop = dwIndex - 1;
	}

	__inline void release(T* pEntry) {
		releaseByIndex(indexOf(pEntry));
	}

	__inline uint32_t indexOfWithRange(T* pEntry) const {
		uint32_t index = (reinterpret_cast<uint32_t>(pEntry) - reinterpret_cast<uint32_t>(m_pObjects)) / m_dwEntrySize;

		if (index < m_dwSize)
			return index;
		else
			return ~0;
	}

	__inline uint32_t indexOf(T* pEntry) {
		return (reinterpret_cast<uint32_t>(pEntry) - reinterpret_cast<uint32_t>(m_pObjects)) / m_dwEntrySize;
	}

	__inline uint32_t findFirst(bool(__cdecl* pfnTest)(T*, void*), void* param) {
		uint8_t* pObject = m_pObjects;
		uint8_t* pFlag = m_pFlags;
		for (uint32_t i = 0; i < m_dwSize; i++, pObject += m_dwEntrySize, pFlag++) {
			if (!(pFlag[0] & 0x80))
				if (pfnTest(reinterpret_cast<T*>(pObject), param))
					return i;
		}
		return 0xFFFFFFFF;
	}

	__inline void forEach(void(__cdecl* pfnDo)(T*, void*), void* param) {
		uint8_t* pObject = m_pObjects;
		uint8_t* pFlag = m_pFlags;
		for (uint32_t i = 0; i < m_dwSize; i++, pObject += m_dwEntrySize, pFlag++) {
			if (!(pFlag[0] & 0x80))
				pfnDo(reinterpret_cast<T*>(pObject), param);
		}
	}

	__inline void forEach(void(__cdecl* pfnDo)(uint32_t dwIndex, T*, void*), void* param) {
		uint8_t* pObject = m_pObjects;
		uint8_t* pFlag = m_pFlags;
		for (uint32_t i = 0; i < m_dwSize; i++, pObject += m_dwEntrySize, pFlag++) {
			if (!(pFlag[0] & 0x80))
				pfnDo(i, reinterpret_cast<T*>(pObject), param);
		}
	}

	// (used by stores)
	// TODO: replace with forEach
	__inline void forEachIndex(void(__cdecl* pfnDo)(uint32_t)) {
		uint8_t* pObject = m_pObjects;
		uint8_t* pFlag = m_pFlags;
		for (uint32_t i = 0; i < m_dwSize; i++, pObject += m_dwEntrySize, pFlag++) {
			if (!(pFlag[0] & 0x80))
				pfnDo(i);
		}
	}

	~atPool() { releaseAll(); }


};

static_assert(sizeof(atPool<uint8_t>) == 0x1C, "Invalid atPool<T> structure size");
