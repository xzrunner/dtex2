#ifndef _DTEX_PKG_MGR_H_
#define _DTEX_PKG_MGR_H_

#include <CU_Singleton.h>

#include <map>
#include <vector>

namespace dtex
{

class Package;

class PkgMgr
{
public:
	bool Add(Package* pkg, int id);
	void Delete(int pkg);

	const Package* Query(int id) const;

	void Clear();

	void SetReleaseTag();
	void ReleaseAfterLastTag();

private:
	std::map<int, Package*> m_packages;

	bool m_release_tag;
	std::vector<int> m_pkg_tag;

	SINGLETON_DECLARATION(PkgMgr);

}; // PkgMgr

}

#endif // _DTEX_PKG_MGR_H_