#ifndef _DTEX_CACHE_MGR_H_
#define _DTEX_CACHE_MGR_H_

#include <CU_Singleton.h>

#include <map>

namespace dtex
{

class Cache;

class CacheMgr
{
public:
	bool Add(Cache* cache, const std::string& key);
	bool Delete(const std::string& key);

	const Cache* Query(const std::string& key) const;

	const std::map<std::string, Cache*>& FetchAll() const { return m_caches; }
	
private:
	std::map<std::string, Cache*> m_caches;

	SINGLETON_DECLARATION(CacheMgr);

}; // CacheMgr

}

#endif // _DTEX_CACHE_MGR_H_