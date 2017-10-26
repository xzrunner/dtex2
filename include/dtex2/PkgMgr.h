#ifndef _DTEX_PKG_MGR_H_
#define _DTEX_PKG_MGR_H_

#include "dtex2/Package.h"

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>

namespace dtex
{

class PkgMgr
{
public:
	bool Add(PackagePtr& pkg, int id);
	void Delete(int pkg);

	const Package* Query(int id) const;

	void Clear();

private:
	CU_MAP<int, PackagePtr> m_packages;

	CU_SINGLETON_DECLARATION(PkgMgr);

}; // PkgMgr

}

#endif // _DTEX_PKG_MGR_H_