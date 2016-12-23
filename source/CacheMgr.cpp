#include "CacheMgr.h"
#include "Cache.h"

#include <string>

namespace dtex
{

SINGLETON_DEFINITION(CacheMgr);

CacheMgr::CacheMgr()
{
}

CacheMgr::~CacheMgr()
{
	std::map<std::string, Cache*>::iterator itr = m_caches.begin();
	for ( ; itr != m_caches.end(); ++itr) {
		delete itr->second;
	}
}

bool CacheMgr::Add(Cache* cache, const std::string& key)
{
	std::pair<std::map<std::string, Cache*>::iterator, bool> ret 
		= m_caches.insert(std::make_pair(key, cache));
	return ret.second;
}

bool CacheMgr::Delete(const std::string& key)
{
	std::map<std::string, Cache*>::iterator itr = m_caches.find(key);
	if (itr == m_caches.end()) {
		return false;
	}

	delete itr->second;
	m_caches.erase(itr);
	return true;
}

const Cache* CacheMgr::Query(const std::string& key) const
{
	std::map<std::string, Cache*>::const_iterator itr = m_caches.find(key);
	if (itr != m_caches.end()) {
		return itr->second;
	} else {
		return NULL;
	}
}

}