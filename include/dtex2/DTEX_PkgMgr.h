#ifndef _DTEX_PKG_MGR_H_
#define _DTEX_PKG_MGR_H_

#include <CU_Singleton.h>

#include <map>

namespace dtex
{

class Package;

class PkgMgr
{
public:
	bool Add(Package* pkg, int id);
	const Package* Query(int id) const;

	void Clear();

private:
	std::map<int, Package*> m_packages;

	SINGLETON_DECLARATION(PkgMgr);

}; // PkgMgr

}

#endif // _DTEX_PKG_MGR_H_