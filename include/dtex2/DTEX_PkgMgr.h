#ifndef _DTEX_PKG_MGR_H_
#define _DTEX_PKG_MGR_H_

#include <cu/cu_macro.h>

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

private:
	std::map<int, Package*> m_packages;

	CU_SINGLETON_DECLARATION(PkgMgr);

}; // PkgMgr

}

#endif // _DTEX_PKG_MGR_H_