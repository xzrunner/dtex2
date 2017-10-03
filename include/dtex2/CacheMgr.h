#ifndef _DTEX_CACHE_MGR_H_
#define _DTEX_CACHE_MGR_H_

#include <cu/cu_macro.h>

#include <map>
#include <string>

namespace dtex
{

class Cache;

class CacheMgr
{
public:
	bool Add(Cache* cache, const std::string& key);
	bool Delete(const std::string& key);

	const Cache* Query(const std::string& key) const;
	void Clear();

	const std::map<std::string, Cache*>& FetchAll() const { return m_caches; }
	
private:
	std::map<std::string, Cache*> m_caches;

	CU_SINGLETON_DECLARATION(CacheMgr);

}; // CacheMgr

}

#endif // _DTEX_CACHE_MGR_H_