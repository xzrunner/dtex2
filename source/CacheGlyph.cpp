#include "CacheGlyph.h"
#include "TextureMid.h"
#include "DebugDraw.h"
#include "RenderAPI.h"
#include "DrawTexture.h"

#include <texpack.h>

#include <algorithm>

namespace dtex
{

static const int BUF_SZ = 128 * 128;
static const int MAX_NODE_SIZE = 256;

static const int PADDING = 1;

CacheGlyph::CacheGlyph(int width, int height, const Callback& cb)
	: m_width(width)
	, m_height(height)
	, m_cb(cb)
{
	m_buf = new uint32_t[BUF_SZ];

	m_bitmap = new uint32_t[width * height];
	m_tex    = new TextureMid(width, height, true);
	m_tp     = texpack_create(width, height, MAX_NODE_SIZE);

	InitDirtyRect();
}

CacheGlyph::~CacheGlyph()
{
	delete[] m_buf;

	delete[] m_bitmap;
	delete m_tex;
	texpack_release(m_tp);
}

void CacheGlyph::DebugDraw() const
{
	DebugDraw::Draw(m_tex->GetID(), 2);
}

void CacheGlyph::Load(uint32_t* bitmap, int width, int height, uint64_t key)
{
	std::set<uint64_t>::iterator itr = m_exists.find(key);
	if (itr != m_exists.end()) {
		return;
	}

	texpack_pos* pos = texpack_add(m_tp, width + PADDING * 2, height + PADDING * 2, false);
	if (!pos) 
	{
		Flush();
		Clear();
		pos = texpack_add(m_tp, width + PADDING * 2, height + PADDING * 2, false);
		if (!pos) {
			return;
		}
	}

	m_exists.insert(key);
	m_nodes.push_back(Node(key, pos));

	int src_ptr = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			uint32_t src = bitmap[src_ptr++];
			uint8_t r = (src >> 24) & 0xff;
			uint8_t g = (src >> 16) & 0xff;
			uint8_t b = (src >> 8) & 0xff;
			uint8_t a = src & 0xff;
			int dst_ptr = (pos->r.ymin + PADDING + y) * m_width + pos->r.xmin + PADDING + x;
			m_bitmap[dst_ptr] = a << 24 | b << 16 | g << 8 | r;
		}
	}

	UpdateDirtyRect(pos);
}

void CacheGlyph::Flush()
{
	if (m_nodes.empty()) {
		return;
	}

	UpdateTexture();

	m_cb.load_start();
	for (int i = 0, n = m_nodes.size(); i < n; ++i) {
		const Node& node = m_nodes[i];
		m_cb.load(m_tex->GetID(), m_tex->GetWidth(), m_tex->GetHeight(), node.GetRect(), node.Key());
	}
	m_cb.load_finish();

	m_exists.clear();
	m_nodes.clear();

	InitDirtyRect();
}

void CacheGlyph::Clear()
{
	memset(m_bitmap, 0, sizeof(uint32_t) * m_width * m_height);
	DrawTexture::Instance()->Clear(m_tex);
	texpack_clear(m_tp);

	m_exists.clear();

	InitDirtyRect();
}

void CacheGlyph::InitDirtyRect()
{
	m_dirty_rect.xmax = m_dirty_rect.ymax = 0;
	m_dirty_rect.xmin = m_width;
	m_dirty_rect.ymin = m_height;
}

void CacheGlyph::UpdateDirtyRect(const texpack_pos* pos)
{
	if (pos->r.xmin < m_dirty_rect.xmin) {
		m_dirty_rect.xmin = pos->r.xmin;
	}
	if (pos->r.ymin < m_dirty_rect.ymin) {
		m_dirty_rect.ymin = pos->r.ymin;
	}
	if (pos->r.xmax > m_dirty_rect.xmax) {
		m_dirty_rect.xmax = pos->r.xmax;
	}
	if (pos->r.ymax > m_dirty_rect.ymax) {
		m_dirty_rect.ymax = pos->r.ymax;
	}
}

void CacheGlyph::UpdateTexture()
{
	int x = m_dirty_rect.xmin,
		y = m_dirty_rect.ymin;
	int w = m_dirty_rect.xmax - m_dirty_rect.xmin,
		h = m_dirty_rect.ymax - m_dirty_rect.ymin;

	if (w * h > BUF_SZ) {
		RenderAPI::UpdateTexture(m_bitmap, m_width, m_height, m_tex->GetID());
		return;
	}
	
	int src = y * m_width + x,
		dst = 0;
	int line_sz = w * sizeof(uint32_t);
	for (int i = 0; i < h; ++i) {
		memcpy(&m_buf[dst], &m_bitmap[src], line_sz);
		src += m_width;
		dst += w;
	}
	RenderAPI::UpdateSubTex(m_buf, x, y, w, h, m_tex->GetID());
}

/************************************************************************/
/* class CacheGlyph::Node                                               */
/************************************************************************/

CacheGlyph::Node::
Node(uint64_t key, texpack_pos* pos) 
	: m_key(key)
{
	m_rect.xmin = pos->r.xmin + PADDING;
	m_rect.ymin = pos->r.ymin + PADDING;
	m_rect.xmax = pos->r.xmax - PADDING;
	m_rect.ymax = pos->r.ymax - PADDING;
}

}