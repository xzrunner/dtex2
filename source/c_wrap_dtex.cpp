#include "dtex2/CacheMgr.h"
#include "dtex2/CachePkgStatic.h"
#include "dtex2/PkgMgr.h"
#include "dtex2/ResourceAPI.h"
#include "dtex2/LoadResTask.h"

#include <assert.h>

namespace dtex
{

extern "C"
void* dtex_query_cache(const char* key)
{
	return const_cast<Cache*>(CacheMgr::Instance()->Query(key));
}

extern "C"
bool dtex_loading_task_empty()
{
	return LoadResTaskMgr::Instance()->IsEmpty();
}

extern "C"
void* dtex_cache_pkg_static_query(const char* key)
{
	return const_cast<Cache*>(CacheMgr::Instance()->Query(key));
}

extern "C"
void* dtex_cache_pkg_static_create(const char* key, int tex_size, int tex_fmt)
{
	CacheMgr* mgr = CacheMgr::Instance();

	// query
	const Cache* old = mgr->Query(key);
	if (old)
	{
		assert(old->Type() == CACHE_PKG_STATIC);
		const CachePkgStatic* ps_old = static_cast<const CachePkgStatic*>(old);
		if (ps_old->GetTexSize() == tex_size) {
			return const_cast<Cache*>(old);
		} else {
			delete old;
			mgr->Delete(key);
		}
	}

	// insert
	Cache* cache = new CachePkgStatic(tex_size, tex_fmt);
	if (cache == nullptr) {
		ResourceAPI::ErrorReload();
		return nullptr;
	}
	bool succ = mgr->Add(cache, key);
	assert(succ);

	return cache;
}

extern "C"
void dtex_cache_pkg_static_delete(const char* key)
{
	CacheMgr* mgr = CacheMgr::Instance();
	const Cache* cache = mgr->Query(key);
	if (cache) {
		delete cache;
		mgr->Delete(key);
	}
}

extern "C"
int dtex_cache_pkg_static_load(void* cache, int pkg_id, int lod)
{
	Cache* c = static_cast<Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	CachePkgStatic* cps = static_cast<CachePkgStatic*>(c);
	const Package* pkg = PkgMgr::Instance()->Query(pkg_id);
	if(!pkg) {
		return 1;  // error
	}

	cps->Load(pkg, lod);
	return 0;  // success
}

extern "C"
int dtex_cache_pkg_static_load_finish(void* cache, bool async)
{
	Cache* c = static_cast<Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	CachePkgStatic* cps = static_cast<CachePkgStatic*>(c);
	return cps->LoadFinish(async);
}

extern "C"
bool dtex_cache_pkg_static_is_empty(const void* cache)
{
	const Cache* c = static_cast<const Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	const CachePkgStatic* cps = static_cast<const CachePkgStatic*>(c);
	return cps->IsEmpty();
}

extern "C"
bool dtex_cache_pkg_static_is_available(const void* cache)
{
	const Cache* c = static_cast<const Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	const CachePkgStatic* cps = static_cast<const CachePkgStatic*>(c);
	return cps->IsAvailable();
}

}