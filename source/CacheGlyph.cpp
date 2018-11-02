#include "dtex2/CacheGlyph.h"
#include "dtex2/TextureMid.h"
#include "dtex2/DebugDraw.h"
#include "dtex2/RenderAPI.h"
#include "dtex2/DrawTexture.h"
#include "dtex2/ResourceAPI.h"
#include "dtex2/TexPacker.h"

#include <cu/cu_stl.h>
#include <unirender/PixelBuffer.h>
#include <unirender/RenderContext.h>
#include <guard/check.h>

#include <algorithm>
#include <cassert>

#include <string.h>

namespace dtex
{

static const int MAX_NODE_SIZE = 512;

static const int PADDING = 1;

CacheGlyph::CacheGlyph(int width, int height, const Callback& cb)
	: m_width(width)
	, m_height(height)
	, m_cb(cb)
{
	m_pages.push_back(std::make_unique<Page>(width, height));
}

void CacheGlyph::DebugDraw() const
{
	DebugDraw::Draw(m_pages[0]->GetTexID(), 2);
}

void CacheGlyph::Clear()
{
	for (auto& page : m_pages) {
		page->Clear();
	}

	m_all_nodes.clear();
	m_new_nodes.clear();
}

void CacheGlyph::Load(const uint32_t* bitmap, int width, int height, uint64_t key)
{
	int pw = width + PADDING * 2;
	int ph = height + PADDING * 2;
	if (!bitmap ||
	    (!(pw <= m_width && ph <= m_height) &&
		 !(ph <= m_width && pw <= m_height))) {
		return;
	}
	if (Exist(key)) {
		return;
	}

	Rect r;

	int page_idx = -1;
	for (size_t i = 0, n = m_pages.size(); i < n; ++i) {
		if (m_pages[i]->AddToTP(pw, ph, r)) {
			page_idx = i;
			break;
		}
	}
	if (page_idx < 0)
	{
		auto page = std::make_unique<Page>(m_width, m_height);
		bool ok = page->AddToTP(pw, ph, r);
		assert(ok);
		page_idx = m_pages.size();
		m_pages.push_back(std::move(page));
	}

	// old version: rebuild
	//if (!m_pages[page_idx]->AddToTP(pw, ph, r))
	//{
	//	Flush();
	//	RenderAPI::Flush();
	//	Clear();
	//	if (!m_pages[page_idx]->AddToTP(pw, ph, r)) {
	//		return;
	//	}
	//}

	auto r_no_padding = r;
	r_no_padding.xmin += PADDING;
	r_no_padding.ymin += PADDING;
	r_no_padding.xmax -= PADDING;
	r_no_padding.ymax -= PADDING;

	Node node({ key, static_cast<size_t>(page_idx), r_no_padding });
	m_all_nodes.insert({ key, node });
	m_new_nodes.push_back(node);

	m_pages[page_idx]->UpdateBitmap(bitmap, width, height, r_no_padding, r);
}

void CacheGlyph::Flush(bool cache_to_c2)
{
	if (m_new_nodes.empty()) {
		return;
	}

	for (auto& p : m_pages) {
		p->UploadTexture();
	}

	if (cache_to_c2)
	{
		m_cb.load_start();
		for (auto& n : m_new_nodes) {
			int tex_id = m_pages[n.page]->GetTexID();
			m_cb.load(tex_id, m_width, m_height, n.region, n.key);
		}
		m_cb.load_finish();
	}

	m_new_nodes.clear();

	// unbind fbo
	RenderAPI::GetRenderContext()->UnbindPixelBuffer();
}

bool CacheGlyph::QueryAndInsert(uint64_t key, float* texcoords, int& tex_id) const
{
	auto itr = m_all_nodes.find(key);
	if (itr == m_all_nodes.end()) {
		return false;
	}

	auto& node = itr->second;
	m_new_nodes.push_back(node);

	tex_id = m_pages[node.page]->GetTexID();

	const Rect& r = node.region;
	float xmin = r.xmin / static_cast<float>(m_width),
		  ymin = r.ymin / static_cast<float>(m_height),
		  xmax = r.xmax / static_cast<float>(m_width),
		  ymax = r.ymax / static_cast<float>(m_height);
	texcoords[0] = xmin; texcoords[1] = ymin;
	texcoords[2] = xmax; texcoords[3] = ymin;
	texcoords[4] = xmax; texcoords[5] = ymax;
	texcoords[6] = xmin; texcoords[7] = ymax;

	return true;
}

void CacheGlyph::GetFirstPageTexInfo(int& id, size_t& w, size_t& h) const
{
	assert(!m_pages.empty());
	auto& p = m_pages.front();
	id = p->GetTexID();
	w = p->GetWidth();
	h = p->GetHeight();
}

bool CacheGlyph::QueryRegion(uint64_t key, int& tex_id, int& xmin, int& ymin, int& xmax, int& ymax) const
{
	auto itr = m_all_nodes.find(key);
	if (itr == m_all_nodes.end()) {
		return false;
	}

	auto& r = itr->second.region;
	xmin = r.xmin;
	ymin = r.ymin;
	xmax = r.xmax;
	ymax = r.ymax;

	tex_id = m_pages[itr->second.page]->GetTexID();

	return true;
}

//////////////////////////////////////////////////////////////////////////
// class CacheGlyph::Page
//////////////////////////////////////////////////////////////////////////

CacheGlyph::Page::Page(size_t width, size_t height)
	: m_width(width)
	, m_height(height)
{
	m_tex = std::make_unique<TextureMid>(width, height, 4, true);
	m_tp = std::make_unique<TexPacker>(width, height, MAX_NODE_SIZE);

	m_pbuf = ur::PixelBuffer::Create(RenderAPI::GetRenderContext(), width, height, ur::TEXTURE_RGBA8, true);
	m_pbuf->Clear();

	InitDirtyRect();
}

bool CacheGlyph::Page::AddToTP(size_t width, size_t height, Rect& ret)
{
	return m_tp->Add(width, height, false, ret);
}

void CacheGlyph::Page::Clear()
{
	DrawTexture::ClearAllTex(m_tex.get());
	m_tp->Clear();
	m_pbuf->Clear();

	InitDirtyRect();
}

int CacheGlyph::Page::GetTexID() const
{
	return m_tex->GetID();
}

void CacheGlyph::Page::UpdateBitmap(const uint32_t* bitmap, int width, int height,
	                                const Rect& pos, const Rect& dirty_r)
{
	uint32_t* bmp_buf = (uint32_t*)m_pbuf->Map(ur::WRITE_ONLY);
	int src_ptr = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			uint32_t src = bitmap[src_ptr++];
			uint8_t r = (src >> 24) & 0xff;
			uint8_t g = (src >> 16) & 0xff;
			uint8_t b = (src >> 8) & 0xff;
			uint8_t a = src & 0xff;

			int dst_ptr = (pos.ymin + y) * m_width + pos.xmin + x;
			bmp_buf[dst_ptr] = a << 24 | b << 16 | g << 8 | r;
		}
	}

	UpdateDirtyRect(dirty_r);
}

void CacheGlyph::Page::UploadTexture()
{
	if (m_dirty_rect.xmin >= m_dirty_rect.xmax ||
		m_dirty_rect.ymin >= m_dirty_rect.ymax) {
		return;
	}

	int x = m_dirty_rect.xmin,
		y = m_dirty_rect.ymin;
	int w = m_dirty_rect.xmax - m_dirty_rect.xmin,
		h = m_dirty_rect.ymax - m_dirty_rect.ymin;
	RenderAPI::SetUnpackRowLength(m_width);
	int offset = (y * m_width + x) * 4;
	m_pbuf->Upload(x, y, w, h, offset, GetTexID());
	RenderAPI::SetUnpackRowLength(0);

	InitDirtyRect();
}

void CacheGlyph::Page::InitDirtyRect()
{
	m_dirty_rect.xmax = m_dirty_rect.ymax = 0;
	m_dirty_rect.xmin = static_cast<int16_t>(m_width);
	m_dirty_rect.ymin = static_cast<int16_t>(m_height);
}

void CacheGlyph::Page::UpdateDirtyRect(const Rect& r)
{
	if (r.xmin < m_dirty_rect.xmin) {
		m_dirty_rect.xmin = r.xmin;
	}
	if (r.ymin < m_dirty_rect.ymin) {
		m_dirty_rect.ymin = r.ymin;
	}
	if (r.xmax > m_dirty_rect.xmax) {
		m_dirty_rect.xmax = r.xmax;
	}
	if (r.ymax > m_dirty_rect.ymax) {
		m_dirty_rect.ymax = r.ymax;
	}
}

}