#include "CacheAPI.h"

namespace dtex
{

static CacheAPI::Callback CB;

void CacheAPI::InitCallback(const Callback& cb)
{
	CB = cb;
}

void CacheAPI::RelocatePkg(int src_pkg, int src_tex, int dst_tex_id, int dst_fmt, int dst_w, int dst_h, int dst_xmin, int dst_ymin, int dst_xmax, int dst_ymax)
{
	CB.relocate_pkg(src_pkg, src_tex, dst_tex_id, dst_fmt, dst_w, dst_h, dst_xmin, dst_ymin, dst_xmax, dst_ymax);
}

void CacheAPI::RelocatePkgFinish()
{
	CB.relocate_pkg_finish();
}

}