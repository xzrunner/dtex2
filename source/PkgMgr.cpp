#include "dtex2/PkgMgr.h"
#include "dtex2/Package.h"

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

bool PkgMgr::Add(PackagePtr& pkg, int id)
{
	auto itr = m_packages.find(id);
	if (itr != m_packages.end()) {
		return false;
	}
	m_packages.insert(std::make_pair(id, std::move(pkg)));
	return true;
}

void PkgMgr::Delete(int pkg)
{
	auto itr = m_packages.find(pkg);
	if (itr != m_packages.end()) {
		m_packages.erase(itr);
	}
}

const Package* PkgMgr::Query(int id) const
{
	auto itr = m_packages.find(id);
	if (itr != m_packages.end()) {
		return itr->second.get();
	} else {
		return NULL;
	}
}

void PkgMgr::Clear()
{
	m_packages.clear();
}

}