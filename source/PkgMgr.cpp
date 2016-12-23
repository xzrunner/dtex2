#include "PkgMgr.h"
#include "Package.h"

namespace dtex
{

SINGLETON_DEFINITION(PkgMgr);

PkgMgr::PkgMgr()
{
}

PkgMgr::~PkgMgr()
{
	for (int i = 0, n = m_packages.size(); i < n; ++i) {
		delete m_packages[i];
	}	
}

bool PkgMgr::Add(Package* pkg, int id)
{
	std::map<int, Package*>::iterator itr = m_packages.find(id);
	if (itr != m_packages.end()) {
		return false;
	}

	m_packages.insert(std::make_pair(id, pkg));
	return true;
}

const Package* PkgMgr::Query(int id) const
{
	std::map<int, Package*>::const_iterator itr = m_packages.find(id);
	if (itr != m_packages.end()) {
		return itr->second;
	} else {
		return NULL;
	}
}

}