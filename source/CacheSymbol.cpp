#include "dtex2/CacheSymbol.h"
#include "dtex2/HardRes.h"
#include "dtex2/TextureMid.h"
#include "dtex2/CS_Node.h"
#include "dtex2/DebugDraw.h"
#include "dtex2/TextureRaw.h"
#include "dtex2/RenderAPI.h"
#include "dtex2/DrawTexture.h"
#include "dtex2/CacheAPI.h"
#include "dtex2/ResourceAPI.h"

#include <texpack.h>

#include <assert.h>

namespace dtex
{

static const int MAX_BLOCK_PRELOAD_COUNT = 1024;

CacheSymbol::CacheSymbol(int width, int height)
	: m_loadable(0)
	, m_clear_block_idx(0)
{
	m_tex = new TextureMid(width, height, 4, true);
	if (m_tex == nullptr) {
		ResourceAPI::ErrorReload();
	}

	int x = 0, y = 0;
	m_block_w = width / BLOCK_X_SZ;
	m_block_h = height / BLOCK_Y_SZ;
	int i = 0;
	for (int iy = 0; iy < BLOCK_Y_SZ; ++iy) {
		for (int ix = 0; ix < BLOCK_X_SZ; ++ix) {
			Block* block = new Block(m_tex, x, y, m_block_w, m_block_h);
			if (block == nullptr) {
				ResourceAPI::ErrorReload();
			}
			m_blocks[i++] = block;
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
	DrawTexture::ClearAllTex(m_tex);

	for (int i = 0, n = BLOCK_X_SZ * BLOCK_Y_SZ; i < n; ++i) {
		m_blocks[i]->Clear();
	}

	// -1 for all
	CacheAPI::OnClearSymBlock(-1);

	m_prenodes.clear();
	m_nodes.clear();

	m_clear_block_idx = 0;
}

void CacheSymbol::LoadStart()
{
	m_loadable++;
}

void CacheSymbol::Load(int tex_id, int tex_w, int tex_h, const Rect& r, uint64_t key, int padding, int extrude, int src_extrude)
{
	if (tex_w <= 0 || tex_h <= 0) {
		return;
	}

	int w = r.xmax - r.xmin,
		h = r.ymax - r.ymin;
	if ((w <= m_block_w && h <= m_block_h) ||
		(w <= m_block_h && h <= m_block_w)) {
		;
	} else {
		return;
	}

	int block_id;
	if (Query(key, block_id)) {
		return;
	}

	m_prenodes.push_back(Prenode(tex_id, tex_w, tex_h, r, key, padding, extrude, src_extrude));	
}

void CacheSymbol::LoadFinish()
{
	if (--m_loadable > 0 || m_prenodes.empty()) {
		return;
	}

	try {
		// insert
		m_prenodes.unique();
		m_prenodes.sort();
		std::list<DrawTask> drawlist;
		std::list<Block*> clearlist;

		auto itr_prenode = m_prenodes.begin();
		for (int i = 0; itr_prenode != m_prenodes.end(); ++itr_prenode, ++i) {
			int block_clear_idx = -1;
			InsertNode(*itr_prenode, drawlist, clearlist, block_clear_idx);
			if (block_clear_idx != -1) {
				CacheAPI::OnClearSymBlock(block_clear_idx);
			}
		}

		// draw clear
		clearlist.unique();
		auto itr_clearlist = clearlist.begin();
		for (; itr_clearlist != clearlist.end(); ++itr_clearlist) {
			ClearBlockTex(*itr_clearlist);
		}

		// draw
		RenderAPI::ScissorDisable();
		drawlist.sort();
		auto itr_drawlist = drawlist.begin();
		for (; itr_drawlist != drawlist.end(); ++itr_drawlist) {
			if (!itr_drawlist->Draw()) {
				throw std::exception();
			}
		}
		DrawTexture::Instance()->Flush();
		RenderAPI::ScissorEnable();

		// clear
		m_prenodes.clear();
	} catch (std::exception&) {
		for (int i = 0, n = BLOCK_X_SZ * BLOCK_Y_SZ; i < n; ++i) {
			m_blocks[i]->Clear();
		}

		// -1 for all
		CacheAPI::OnClearSymBlock(-1);

		m_nodes.clear();

		m_clear_block_idx = 0;
	}
}

const CS_Node* CacheSymbol::Query(uint64_t key, int& block_id) const
{
	for (int i = 0, n = BLOCK_X_SZ * BLOCK_Y_SZ; i < n; ++i) 
	{
		int idx = m_blocks[i]->Query(key);
		if (idx != -1) {
			block_id = i;
			assert(idx >= 0 && static_cast<size_t>(idx) < m_nodes.size());
			return &m_nodes[idx];
		}
	}
	block_id = -1;
	return nullptr;
}

int CacheSymbol::GetTexID() const
{
	return m_tex->GetID();
}

void CacheSymbol::ClearBlockData()
{
	Block* b = m_blocks[m_clear_block_idx];
	b->Clear();
	CacheAPI::OnClearSymBlock(m_clear_block_idx);

	++m_clear_block_idx;
	if (m_clear_block_idx >= BLOCK_X_SZ * BLOCK_Y_SZ) {
		m_clear_block_idx -= BLOCK_X_SZ * BLOCK_Y_SZ;
	}
}

void CacheSymbol::ClearBlockTex(const Block* b)
{
	float offx = static_cast<float>(b->OffX()),
		  offy = static_cast<float>(b->OffY());
	int tex_w = m_tex->GetWidth(),
		tex_h = m_tex->GetHeight();
	float xmin = offx / tex_w,
		  ymin = offy / tex_h,
		  xmax = (offx + m_block_w) / tex_w,
		  ymax = (offy + m_block_h) / tex_h;
	DrawTexture::Instance()->ClearTex(m_tex, xmin, ymin, xmax, ymax);
}

bool CacheSymbol::InsertNode(const Prenode& prenode, std::list<DrawTask>& drawlist, 
	                         std::list<Block*>& clearlist, int& clear_block_idx)
{
	clear_block_idx = -1;

	int extend = prenode.Padding() + prenode.Extrude();

	Block* block = nullptr;
	texpack_pos* pos = nullptr;
	for (int i = 0, n = BLOCK_X_SZ * BLOCK_Y_SZ; i < n; ++i) {
		pos = m_blocks[i]->Insert(prenode, extend);
		if (pos) {
			block = m_blocks[i];
			break;
		}
	}

	if (!pos) 
	{
		Block* cleared = m_blocks[m_clear_block_idx];
		ClearBlockData();
		clear_block_idx = m_clear_block_idx;

		clearlist.push_back(cleared);
		// update drawlist
		CU_LIST<DrawTask>::iterator itr = drawlist.begin();
		for ( ; itr != drawlist.end(); ) 
		{
			if (itr->GetBlock() == cleared) {
				itr = drawlist.erase(itr);
			} else {
				++itr;
			}
		}

		pos = cleared->Insert(prenode, extend);
		if (!pos) {
			return false;
		} else {
			block = cleared;
		}
	}

	CS_Node node(prenode.Key(), m_tex, pos);
	m_nodes.push_back(node);
	
	int src_extrude = prenode.SrcExtrude();

	Rect src_r = prenode.GetRect();
	src_r.xmin -= src_extrude;
	src_r.ymin -= src_extrude;
	src_r.xmax += src_extrude;
	src_r.ymax += src_extrude;

	Rect dst_r;
	dst_r.xmin = pos->r.xmin - src_extrude;
	dst_r.ymin = pos->r.ymin - src_extrude;
	dst_r.xmax = pos->r.xmax + src_extrude;
	dst_r.ymax = pos->r.ymax + src_extrude;

	drawlist.push_back(DrawTask(m_tex, block, prenode, src_r, dst_r, pos->is_rotated));

	block->Insert(node.Key(), m_nodes.size() - 1);

	return true;
}

/************************************************************************/
/* class CacheSymbol::Prenode                                           */
/************************************************************************/

CacheSymbol::Prenode::
Prenode(int tex_id, int tex_w, int tex_h, const Rect& r, uint64_t key, int padding, int extrude, int src_extrude)
	: m_tex_id(tex_id)
	, m_tex_w(tex_w)
	, m_tex_h(tex_h)
	, m_rect(r)
	, m_key(key)
	, m_padding(padding)
	, m_extrude(extrude)
	, m_src_extrude(src_extrude)
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
Query(uint64_t key) const
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
		return nullptr;
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
Insert(uint64_t key, int val)
{
	m_lut.Insert(NodeLUT::Node(key, val));
}

/************************************************************************/
/* class CacheSymbol::DrawTask                                          */
/************************************************************************/

CacheSymbol::DrawTask::
DrawTask(Texture* tex, Block* block, const Prenode& pn, const Rect& src, const Rect& dst, bool rotate)
	: m_tex(tex)
	, m_block(block)
	, m_pn(&pn)
	, m_src(src)
	, m_dst(dst)
	, m_rotate(rotate)
{
}

bool CacheSymbol::DrawTask::
Draw() const
{
	if (!DrawTexture::Instance()->Draw(m_pn->TexID(), m_pn->TexWidth(), m_pn->TexHeight(), m_src, m_tex, m_dst, m_rotate)) {
		return false;
	}
	if (m_pn->Extrude() != 0) {
		if (!DrawExtrude(m_pn->TexID(), m_pn->TexWidth(), m_pn->TexHeight(), m_src, m_dst, m_rotate, m_pn->Extrude())) {
			return false;
		}
	}
	return true;
}

bool CacheSymbol::DrawTask::
DrawExtrude(int src_tex_id, int src_w, int src_h, const Rect& src_r, const Rect& dst_r, bool rotate, int extrude) const
{
	static const int SRC_EXTRUDE = 1;

	DrawTexture* dt = DrawTexture::Instance();

	Rect src, dst;
	if (!rotate)
	{
		// left
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = 0; src.ymax = src_h;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// right
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w - SRC_EXTRUDE; src.ymin = 0; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// top
		src.xmin = 0; src.xmax = src_w; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.ymin = dst.ymax; dst.ymax += extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// bottom
		src.xmin = 0; src.xmax = src_w; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.ymax = dst.ymin; dst.ymin -= extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// left-top
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= extrude; dst.ymin = dst.ymax; dst.ymax += extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// right-top
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += extrude; dst.ymin = dst.ymax; dst.ymax += extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// left-bottom
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= extrude; dst.ymax = dst.ymin; dst.ymin -= extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// right-bottom
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += extrude; dst.ymax = dst.ymin; dst.ymin -= extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}
	} 
	else
	{
		// left
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = 0; src.ymax = src_h;
		dst = dst_r; dst.ymin = dst.ymax; dst.ymax += extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// right
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = 0; src.ymax = src_h;
		dst = dst_r; dst.ymax = dst.ymin; dst.ymin -= extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// top
		src.xmin = 0; src.xmax = src_w; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// bottom
		src.xmin = 0; src.xmax = src_w; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// left-top
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += extrude; dst.ymin = dst.ymax; dst.ymax += extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// right-top
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = src_h - SRC_EXTRUDE; src.ymax = src_h;
		dst = dst_r; dst.xmin = dst.xmax; dst.xmax += extrude; dst.ymax = dst.ymin; dst.ymin -= extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// left-bottom
		src.xmin = 0; src.xmax = SRC_EXTRUDE; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= extrude; dst.ymin = dst.ymax; dst.ymax += extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}

		// right-bottom
		src.xmin = src_w - SRC_EXTRUDE; src.xmax = src_w; src.ymin = 0; src.ymax = SRC_EXTRUDE;
		dst = dst_r; dst.xmax = dst.xmin; dst.xmin -= extrude; dst.ymax = dst.ymin; dst.ymin -= extrude;
		if (!dt->Draw(src_tex_id, src_w, src_h, src, m_tex, dst, rotate)) {
			return false;
		}
	}

	return true;
}

}
