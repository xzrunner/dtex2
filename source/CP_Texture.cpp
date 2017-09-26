#include "CP_Texture.h"
#include "CP_Prenode.h"
#include "DTEX_Texture.h"
#include "TextureMid.h"
#include "DTEX_Package.h"
#include "DrawTexture.h"
#include "ResourceAPI.h"

#include <texpack.h>

#include <assert.h>
#include <stdlib.h>

namespace dtex
{

CP_Texture::CP_Texture(int edge, int tp_node_count)
	: m_tex(NULL)
	, m_tp(NULL)
	, m_ud(NULL)
{
	m_tex = new TextureMid(edge, edge, false);
	if (m_tex == NULL) {
		ResourceAPI::ErrorReload();
	}

	m_region.xmin = m_region.ymin = 0;
	m_region.xmax = m_region.ymax = edge;

	m_tp = texpack_create(edge, edge, tp_node_count);
}

CP_Texture::~CP_Texture()
{
	delete m_tex;

	for (int i = 0, n = m_nodes.size(); i < n; ++i) {
		delete m_nodes[i];
	}

	texpack_release(m_tp);

	if (m_ud) {
		uint8_t* pixels = static_cast<uint8_t*>(m_ud);
		free(pixels);
		m_ud = NULL;
	}
}

void CP_Texture::Clear(bool clear_tex)
{
	if (m_nodes.empty()) {
		return;
	}

	if (clear_tex) {
		DrawTexture::ClearAllTex(m_tex);
	}

	for (int i = 0, n = m_nodes.size(); i < n; ++i) {
		delete m_nodes[i];
	}
	m_nodes.clear();

	assert(m_tp);
	texpack_clear(m_tp);
}

bool CP_Texture::PackPrenode(const CP_Prenode& prenode, float scale, Cache* cache)
{
	const Package* pkg = prenode.GetPackage();
	int tex_idx = prenode.GetTexIdx();

	const Texture* tex = pkg->GetTexture(tex_idx);
	int w = static_cast<int>(tex->GetWidth() * prenode.GetScale() * scale),
		h = static_cast<int>(tex->GetHeight() * prenode.GetScale() * scale);
	switch (prenode.GetLod())
	{
	case 1:
		w /= 2;
		h /= 2;
		break;
	case 2:
		w /= 4;
		h /= 4;
		break;
	}

	struct texpack_pos* pos = NULL;
	if (w >= h) {
		pos = texpack_add(m_tp, w, h, true);
	} else {
		pos = texpack_add(m_tp, h, w, false);
	}
	if (!pos) {
//		LOGW("%s", "cf insert fail.");
		return false;
	}

	pos->r.xmin += m_region.xmin;
	pos->r.xmax += m_region.xmin;
	pos->r.ymin += m_region.ymin;
	pos->r.ymax += m_region.ymin;

	Rect r;
	r.xmin = pos->r.xmin;
	r.xmax = pos->r.xmax;
	r.ymin = pos->r.ymin;
	r.ymax = pos->r.ymax;

	bool rotate = (pos->is_rotated && w >= h) || (!pos->is_rotated && h >= w);
	CP_Node* node = new CP_Node(pkg, tex_idx, prenode.GetLod(), this, r, rotate, cache);
	if (node == NULL) {
		ResourceAPI::ErrorReload();
	}
	m_nodes.push_back(node);

	return true;
}

}