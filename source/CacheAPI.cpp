#include "dtex2/CacheAPI.h"

namespace dtex
{

static CacheAPI::Callback CB;

void CacheAPI::InitCallback(const Callback& cb)
{
	CB = cb;
}

void CacheAPI::RelocatePkg(int src_pkg, int src_tex, int src_lod, int dst_tex_id, int dst_fmt, int dst_w, int dst_h, int dst_xmin, int dst_ymin, int dst_xmax, int dst_ymax)
{
	if (CB.relocate_pkg) {
		CB.relocate_pkg(src_pkg, src_tex, src_lod, dst_tex_id, dst_fmt, dst_w, dst_h, dst_xmin, dst_ymin, dst_xmax, dst_ymax);
	}
}

void CacheAPI::RelocatePkgFinish()
{
	if (CB.relocate_pkg_finish) {
		CB.relocate_pkg_finish();
	}
}

void CacheAPI::RemoveTex(int tex_id)
{
	if (CB.remove_tex) {
		CB.remove_tex(tex_id);
	}
}

void CacheAPI::OnClearSymBlock(int block_id)
{
	if (CB.on_clear_sym_block) {
		CB.on_clear_sym_block(block_id);
	}
}

}