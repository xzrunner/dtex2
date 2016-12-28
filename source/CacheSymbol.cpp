#include "CacheSymbol.h"
#include "HardRes.h"
#include "TextureMid.h"
#include "CS_Node.h"
#include "DebugDraw.h"
#include "TextureRaw.h"

#include <texpack.h>

#include <assert.h>

namespace dtex
{

static const int MAX_BLOCK_PRELOAD_COUNT = 1024;

CacheSymbol::CacheSymbol(int width, int height, int padding, int extrude)
	: m_loadable(0)
	, m_padding(padding)
	, m_extrude(extrude)
	, m_clear_block_idx(0)
{
	m_tex = new TextureMid(width, height, true);

	int x = 0, y = 0;
	m_block_w = width / BLOCK_X_SZ;
	m_block_h = height / BLOCK_Y_SZ;
	int i = 0;
	for (int iy = 0; iy < BLOCK_Y_SZ; ++iy) {
		for (int ix = 0; ix < BLOCK_X_SZ; ++ix) {
			m_blocks[i++] = new Block(m_tex, x, y, m_block_w, m_block_h);
			x += m_block_w;
		}
		x = 0;
		y += m_block_h;
	}
}

CacheSymbol::~CacheSymbol()
{
	delete m_tex;

	for (int i = 0, n = BLOCK_X_SZ * BLOCK_Y_SZ; i < n; ++i) {
		delete m_blocks[i];
	}
}

void CacheSymbol::DebugDraw() const
{
	DebugDraw::Draw(m_tex->GetID(), 4);
}

// only clear texture
void CacheSymbol::Clear()
{
	m_tex->Clear();

	for (int i = 0, n = BLOCK_X_SZ * BLOCK_Y_SZ; i < n; ++i) {
		m_blocks[i]->Clear();
	}
}

void CacheSymbol::LoadStart()
{
	m_loadable++;
}

void CacheSymbol::Load(int tex_id, int tex_w, int tex_h, const Rect& r, uint32_t key)
{
	if (tex_id <= 0 || tex_w <= 0 || tex_h <= 0) {
		return;
	}

	if (Query(key)) {
		return;
	}

	m_prenodes.push_back(Prenode(tex_id, tex_w, tex_h, r, key));	
}

void CacheSymbol::LoadFinish()
{
	if (--m_loadable > 0 || m_prenodes.empty()) {
		return;
	}

	m_prenodes.unique();
	m_prenodes.sort();

	std::list<Prenode>::iterator itr = m_prenodes.begin();
	for ( ; itr != m_prenodes.end(); ++itr) {
		InsertNode(*itr);
	}

	m_prenodes.clear();
}

const CS_Node* CacheSymbol::Query(uint32_t key) const
{
	for (int i = 0, n = BLOCK_X_SZ * BLOCK_Y_SZ; i < n; ++i) 
	{
		int idx = m_blocks[i]->Query(key);
		if (idx != -1) {
			assert(idx >= 0 && idx < m_nodes.size());
			return &m_nodes[idx];
		}
	}
	return NULL;
}

int CacheSymbol::GetTexID() const
{
	return m_tex->GetID();
}

void CacheSymbol::ClearBlock()
{
	Block* b = m_blocks[m_clear_block_idx];
	b->Clear();

	int tex_w = m_tex->GetWidth(),
		tex_h = m_tex->GetHeight();
	float xmin = (float)(b->OffX()) / tex_w,
		  ymin = (float)(b->OffY()) / tex_h,
		  xmax = (float)(b->OffX() + m_block_w) / tex_w,
		  ymax = (float)(b->OffY() + m_block_h) / tex_h;
	m_tex->Clear(xmin, ymin, xmax, ymax);
	
	++m_clear_block_idx;
	if (m_clear_block_idx >= BLOCK_X_SZ * BLOCK_Y_SZ) {
		m_clear_block_idx -= BLOCK_X_SZ * BLOCK_Y_SZ;
	}
}

bool CacheSymbol::InsertNode(const Prenode& prenode)
{
	if (prenode.Width() > m_block_w && prenode.Height() > m_block_h ||
		prenode.Width() > m_block_h && prenode.Height() > m_block_w) {
		return false;
	}

	Block* block = NULL;
	texpack_pos* pos = NULL;
	for (int i = 0, n = BLOCK_X_SZ * BLOCK_Y_SZ; i < n; ++i) {
		pos = m_blocks[i]->Insert(prenode, m_padding + m_extrude);
		if (pos) {
			block = m_blocks[i];
			break;
		}
	}

	if (!pos) {
		Block* cleared = m_blocks[m_clear_block_idx];
		ClearBlock();
		pos = cleared->Insert(prenode, m_padding + m_extrude);
		if (!pos) {
			return false;
		} else {
			block = cleared;
		}
	}

	CS_Node node(prenode.Key(), m_tex, pos);
	m_nodes.push_back(node);

	Rect dst_r;
	dst_r.xmin = pos->r.xmin;
	dst_r.ymin = pos->r.ymin;
	dst_r.xmax = pos->r.xmax;
	dst_r.ymax = pos->r.ymax;

	m_tex->DrawFrom(prenode.TexID(), prenode.Width(), prenode.Height(), prenode.GetRect(), dst_r, pos->is_rotated);
	if (m_extrude != 0) {
		DrawExtrude(prenode.TexID(), prenode.Width(), prenode.Height(), prenode.GetRect(), dst_r, pos->is_rotated);
	}

	block->Insert(node.Key(), m_nodes.size() - 1);

	return true;
}

void CacheSymbol::DrawExtrude(int src_tex_id, int src_w, int src_h, const Rect& src_r, const Rect& dst_r, bool rotate) const
{
	static const int SRC_EXTRUDE = 1;

	Rect src, dst;
	if (!rotate)
	{
		// left
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = 0; src.ymax = src_h;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// right
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = 0; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// top
		src.xmin = 0; src.xmax = src_w; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.ymin = dst.ymax; dst.ymax += m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// bottom
		src.xmin = 0; src.xmax = src_w; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.ymax = dst.ymin; dst.ymin -= m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// left-top
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= m_extrude; dst.ymin = dst.ymax; dst.ymax += m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// right-top
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += m_extrude; dst.ymin = dst.ymax; dst.ymax += m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// left-bottom
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= m_extrude; dst.ymax = dst.ymin; dst.ymin -= m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// right-bottom
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += m_extrude; dst.ymax = dst.ymin; dst.ymin -= m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);
	} 
	else
	{
		// left
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = 0; src.ymax = src_h;
		dst = dst_r; dst.ymin = dst.ymax; dst.ymax += m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// right
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = 0; src.ymax = src_h;
		dst = dst_r; dst.ymax = dst.ymin; dst.ymin -= m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// top
		src.xmin = 0; src.xmax = src_w; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// bottom
		src.xmin = 0; src.xmax = src_w; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// left-top
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += m_extrude; dst.ymin = dst.ymax; dst.ymax += m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// right-top
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += m_extrude; dst.ymax = dst.ymin; dst.ymin -= m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// left-bottom
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= m_extrude; dst.ymin = dst.ymax; dst.ymax += m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);

		// right-bottom
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= m_extrude; dst.ymax = dst.ymin; dst.ymin -= m_extrude;
		m_tex->DrawFrom(src_tex_id, src_w, src_h, src, dst, rotate);
	}
}

/************************************************************************/
/* class CacheSymbol::Prenode                                           */
/************************************************************************/

CacheSymbol::Prenode::
Prenode(int tex_id, int tex_w, int tex_h, const Rect& r, uint32_t key)
	: m_tex_id(tex_id)
	, m_tex_w(tex_w)
	, m_tex_h(tex_h)
	, m_rect(r)
	, m_key(key)
{
}

/************************************************************************/
/* class CacheSymbol::Block                                             */
/************************************************************************/

CacheSymbol::Block::
Block(Texture* tex, int x, int y, int w, int h)
	: m_tex(tex)
	, m_x(x)
	, m_y(y)
{
	m_tp = texpack_create(w, h, MAX_BLOCK_PRELOAD_COUNT);
}

CacheSymbol::Block::
~Block()
{
	texpack_release(m_tp);
}

void CacheSymbol::Block::
Clear()
{
	texpack_clear(m_tp);
	m_lut.Clear();
}

int CacheSymbol::Block::
Query(uint32_t key) const
{
	return m_lut.Query(key);
}

texpack_pos* CacheSymbol::Block::
Insert(const Prenode& node, int extend)
{
	int w = node.Width() + extend * 2,
		h = node.Height() + extend * 2;
	texpack_pos* pos = texpack_add(m_tp, w, h, true);
	if (!pos) {
		return false;
	}

	pos->r.xmin += extend;
	pos->r.ymin += extend;
	pos->r.xmax -= extend;
	pos->r.ymax -= extend;

	pos->r.xmin += m_x;
	pos->r.xmax += m_x;
	pos->r.ymin += m_y;
	pos->r.ymax += m_y;

	return pos;
}

void CacheSymbol::Block::
Insert(uint32_t key, int val)
{
	m_lut.Insert(NodeLUT::Node(key, val));
}

}