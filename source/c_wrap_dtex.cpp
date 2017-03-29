#include "CacheMgr.h"
#include "CachePkgStatic.h"
#include "DTEX_PkgMgr.h"
#include "AsyncTask.h"

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
	return AsyncTask::Instance()->IsEmpty();
}

extern "C"
void* dtex_cache_pkg_static_query(const char* key)
{
	return const_cast<Cache*>(CacheMgr::Instance()->Query(key));
}

extern "C"
void* dtex_cache_pkg_static_create(const char* key, int tex_size, int tex_count)
{
	CacheMgr* mgr = CacheMgr::Instance();

	// query
	const Cache* old = mgr->Query(key);
	if (old)
	{
		assert(old->Type() == CACHE_PKG_STATIC);
		const CachePkgStatic* ps_old = static_cast<const CachePkgStatic*>(old);
		if (ps_old->GetTexSize() == tex_size && ps_old->GetTexCount() == tex_count) {
			return const_cast<Cache*>(old);
		} else {
			delete old;
			mgr->Delete(key);
		}
	}

	// insert
	Cache* cache = new CachePkgStatic(tex_size, tex_count);
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
void dtex_cache_pkg_static_load(void* cache, int pkg_id)
{
	Cache* c = static_cast<Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	CachePkgStatic* cps = static_cast<CachePkgStatic*>(c);
	const Package* pkg = PkgMgr::Instance()->Query(pkg_id);
	cps->Load(pkg);
}

extern "C"
void dtex_cache_pkg_static_load_finish(void* cache, bool async)
{
	Cache* c = static_cast<Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	CachePkgStatic* cps = static_cast<CachePkgStatic*>(c);
	cps->LoadFinish(async);
}

extern "C"
bool dtex_cache_pkg_static_is_empty(const void* cache)
{
	const Cache* c = static_cast<const Cache*>(cache);
	assert(c->Type() == CACHE_PKG_STATIC);
	const CachePkgStatic* cps = static_cast<const CachePkgStatic*>(c);
	return cps->IsEmpty();
}

}