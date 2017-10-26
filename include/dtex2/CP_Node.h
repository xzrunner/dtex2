#ifndef _DTEX_CACHE_PACKAGE_NODE_H_
#define _DTEX_CACHE_PACKAGE_NODE_H_

#include "dtex2/Math.h"

#include <cu/uncopyable.h>

namespace dtex
{

class Package;
class CP_Texture;

class CP_Node : private cu::Uncopyable
{
public:
	CP_Node(const Package* src_pkg, int src_tex_idx, int src_lod,
		CP_Texture* dst_tex, const Rect& dst_rect, bool dst_rotate, void* ud);
	// todo
// 	CP_Node(const CP_Node& node);
// 	CP_Node& operator = (const CP_Node& node);

	const Package* GetSrcPkg() const { return m_src_pkg; }
	int GetSrcTexIdx() const { return m_src_tex_idx; }
	int GetSrcLod() const { return m_src_lod; }

	CP_Texture* GetDstTex() const { return m_dst_tex; }
	const Rect& GetDstRect() const { return m_dst_rect; }

	void* GetUD() const { return m_ud; }

private:
	const Package* m_src_pkg;
	int            m_src_tex_idx;
	int            m_src_lod;

	CP_Texture* m_dst_tex;
	Rect        m_dst_rect;
	bool        m_dst_rotated;

	// ?
	bool        m_finish; // relocated

	void*       m_ud;

}; // CP_Node

}

#endif // _DTEX_CACHE_PACKAGE_NODE_H_