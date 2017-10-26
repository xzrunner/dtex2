#include "dtex2/Facade.h"
#include "dtex2/CacheMgr.h"
#include "dtex2/DrawTexture.h"
#include "dtex2/PkgMgr.h"
#include "dtex2/ResCache.h"
#include "dtex2/LoadResTask.h"

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