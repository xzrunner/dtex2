#include "CacheMgr.h"
#include "CachePkgStatic.h"
#include "PkgMgr.h"

#include <assert.h>

namespace dtex
{

extern "C"
void* dtex_query_cache(const char* key)
{
	return const_cast<Cache*>(CacheMgr::Instance()->Query(key));
}

extern "C"
void* dtex_create_cache_pkg_static(const char* key, int tex_size, int tex_count)
{
	CacheMgr* mgr = CacheMgr::Instance();

	Cache* cache = new CachePkgStatic(tex_size, tex_count);
	bool succ = mgr->Add(cache, key);
	if (succ) {
		return cache;
	}

	const Cache* old = mgr->Query(key);
	assert(old->Type() == CACHE_PKG_STATIC);
	const CachePkgStatic* _old = static_cast<const CachePkgStatic*>(old);
	if (_old->GetTexSize() == tex_size && _old->GetTexCount() == tex_count) {
		delete cache;
		return const_cast<Cache*>(old);
	}

	mgr->Delete(key);
	mgr->Add(cache, key);

	return cache;
}

extern "C"
void dtex_cache_pkg_static_load(void* cache, int pkg_id)
{
	Cache* c = static_cast<Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	CachePkgStatic* cps = static_cast<CachePkgStatic*>(c);
	const Package* pkg = PkgMgr::Instance()->Query(pkg_id);
	cps->Load(pkg);
}

extern "C"
void dtex_cache_pkg_static_load_finish(void* cache)
{
	Cache* c = static_cast<Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	CachePkgStatic* cps = static_cast<CachePkgStatic*>(c);
	cps->LoadFinish();
}

}