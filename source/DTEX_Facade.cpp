#include "DTEX_Facade.h"
#include "CacheMgr.h"
#include "DrawTexture.h"
#include "DTEX_PkgMgr.h"
#include "ResCache.h"
#include "LoadResTask.h"

namespace dtex
{

void Facade::Clear()
{
	CacheMgr::Instance()->Clear();
	DrawTexture::Instance()->Clear();
	PkgMgr::Instance()->Clear();
	ResCache::Instance()->Clear();
}

void Facade::Flush()
{
	LoadResTaskMgr::Instance()->Flush();
}

}