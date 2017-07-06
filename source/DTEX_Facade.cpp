#include "DTEX_Facade.h"
#include "CacheMgr.h"
#include "DrawTexture.h"
#include "DTEX_PkgMgr.h"
#include "AsyncTask.h"
#include "ResCache.h"

namespace dtex
{

void Facade::Clear()
{
	CacheMgr::Instance()->Clear();
	DrawTexture::Instance()->Clear();
	PkgMgr::Instance()->Clear();
	AsyncTask::Instance()->Clear();
	ResCache::Instance()->Clear();
}

}