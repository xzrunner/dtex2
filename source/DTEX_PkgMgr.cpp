#include "DTEX_PkgMgr.h"
#include "DTEX_Package.h"

namespace dtex
{

CU_SINGLETON_DEFINITION(PkgMgr);

PkgMgr::PkgMgr()
{
}

PkgMgr::~PkgMgr()
{
	Clear();
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

void PkgMgr::Delete(int pkg)
{
	std::map<int, Package*>::iterator itr = m_packages.find(pkg);
	if (itr != m_packages.end()) {
		delete itr->second;
		m_packages.erase(itr);
	}
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

void PkgMgr::Clear()
{
	std::map<int, Package*>::iterator itr = m_packages.begin();
	for ( ; itr != m_packages.end(); ++itr) {
		delete itr->second;
	}
	m_packages.clear();
}

}