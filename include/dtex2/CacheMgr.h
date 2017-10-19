#ifndef _DTEX_CACHE_MGR_H_
#define _DTEX_CACHE_MGR_H_

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>

namespace dtex
{

class Cache;

class CacheMgr
{
public:
	bool Add(Cache* cache, const CU_STR& key);
	bool Delete(const CU_STR& key);

	const Cache* Query(const CU_STR& key) const;
	void Clear();

	const CU_MAP<CU_STR, Cache*>& FetchAll() const { return m_caches; }
	
private:
	CU_MAP<CU_STR, Cache*> m_caches;

	CU_SINGLETON_DECLARATION(CacheMgr);

}; // CacheMgr

}

#endif // _DTEX_CACHE_MGR_H_