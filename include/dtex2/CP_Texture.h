#ifndef _DTEX_CACHE_PACKAGE_TEXTURE_H_
#define _DTEX_CACHE_PACKAGE_TEXTURE_H_

#include "dtex2/typedef.h"
#include "dtex2/CP_Node.h"

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

struct texpack;

namespace dtex
{

class Texture;
class CP_Prenode;
class Cache;

class CP_Texture : private cu::Uncopyable
{
public:
	CP_Texture(int edge, int bpp, int tp_node_count);
	~CP_Texture();

	void Clear(bool clear_tex = true);

	Texture* GetTexture() const { return m_tex; }

	bool PackPrenode(const CP_Prenode& prenode, float scale, Cache* cache);

	const CU_VEC<CP_Node*>& GetNodes() const { return m_nodes; }

	void* GetUD() const { return m_ud; }
	void  SetUD(void* ud) { m_ud = ud; }

private:
	Texture* m_tex;
	Rect     m_region;

	CU_VEC<CP_Node*> m_nodes;

	texpack* m_tp;

	void* m_ud;

}; // CP_Texture

}

#endif // _DTEX_CACHE_PACKAGE_TEXTURE_H_