#include "dtex2/CacheMgr.h"
#include "dtex2/Cache.h"

namespace dtex
{

CU_SINGLETON_DEFINITION(CacheMgr);

CacheMgr::CacheMgr()
{
}

CacheMgr::~CacheMgr()
{
	CU_MAP<CU_STR, Cache*>::iterator itr = m_caches.begin();
	for ( ; itr != m_caches.end(); ++itr) {
		delete itr->second;
	}
}

bool CacheMgr::Add(Cache* cache, const CU_STR& key)
{
	std::pair<CU_MAP<CU_STR, Cache*>::iterator, bool> ret 
		= m_caches.insert(std::make_pair(key, cache));
	return ret.second;
}

bool CacheMgr::Delete(const CU_STR& key)
{
	CU_MAP<CU_STR, Cache*>::iterator itr = m_caches.find(key);
	if (itr == m_caches.end()) {
		return false;
	} else {
		m_caches.erase(itr);
		return true;
	}
}

const Cache* CacheMgr::Query(const CU_STR& key) const
{
	CU_MAP<CU_STR, Cache*>::const_iterator itr = m_caches.find(key);
	if (itr != m_caches.end()) {
		return itr->second;
	} else {
		return NULL;
	}
}

void CacheMgr::Clear()
{
	CU_MAP<CU_STR, Cache*>::iterator itr = m_caches.begin();
	for ( ; itr != m_caches.end(); ++itr) {
		itr->second->Clear();
	}
}

}