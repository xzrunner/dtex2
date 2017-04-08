#include "DTEX_PkgMgr.h"
#include "DTEX_Package.h"

namespace dtex
{

SINGLETON_DEFINITION(PkgMgr);

PkgMgr::PkgMgr()
	: m_release_tag(false)
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
	if (m_release_tag) {
		m_pkg_tag.push_back(id);
	}
	return true;
}

void PkgMgr::Delete(int pkg)
{
	std::map<int, Package*>::iterator itr = m_packages.find(pkg);
	if (itr != m_packages.end()) {
		delete itr->second;
		m_packages.erase(itr);
	}

	for (int i = 0, n = m_pkg_tag.size(); i < n; ++i) {
		if (m_pkg_tag[i] == pkg) {
			m_pkg_tag.erase(m_pkg_tag.begin() + i);
			break;
		}
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

void PkgMgr::SetReleaseTag()
{
	m_release_tag = true;
}

void PkgMgr::ReleaseAfterLastTag()
{
	for (int i = 0, n = m_pkg_tag.size(); i < n; ++i) {
		std::map<int, Package*>::iterator itr = m_packages.find(m_pkg_tag[i]);
		delete itr->second;
		m_packages.erase(itr);
	}
	m_pkg_tag.clear();
	m_release_tag = false;
}

}