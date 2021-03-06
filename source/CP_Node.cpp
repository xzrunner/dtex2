#include "dtex2/CP_Node.h"

namespace dtex
{

CP_Node::CP_Node(const Package* src_pkg, int src_tex_idx, int src_lod,
				 CP_Texture* dst_tex, const Rect& dst_rect, bool dst_rotate, void* ud)
	: m_src_pkg(src_pkg)
	, m_src_tex_idx(src_tex_idx)
	, m_src_lod(src_lod)
	, m_dst_tex(dst_tex)
	, m_dst_rect(dst_rect)
	, m_dst_rotated(dst_rotate)
	, m_finish(false)
	, m_ud(ud)
{
}

}