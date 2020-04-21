#ifndef _DTEX_CACHE_GLYPH_H_
#define _DTEX_CACHE_GLYPH_H_

#include "dtex2/Cache.h"
#include "dtex2/CacheType.h"
#include "dtex2/typedef.h"

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>
#include <unirender2/typedef.h>

#include <memory>

#include <boost/noncopyable.hpp>

#include <stdint.h>

namespace ur2 { class Device; class Context; class WritePixelBuffer; }

namespace dtex
{

class Texture;
class TexPacker;

class CacheGlyph : public Cache, private cu::Uncopyable
{
public:
	struct Callback
	{
		void (*load_start)();
		void (*load)(int tex_id, int tex_w, int tex_h, const Rect& r, uint64_t key);
		void (*load_finish)();
	};

public:
	CacheGlyph(const ur2::Device& dev, int width, int height, const Callback& cb);

	virtual int Type() const override { return CACHE_GLYPH; }
	virtual void DebugDraw() const override;
	virtual void Clear() override;

	void Load(const ur2::Device& dev, ur2::Context& ctx, const uint32_t* bitmap,
        int width, int height, uint64_t key);
	bool Flush(ur2::Context& ctx, bool cache_to_c2);

	// query from cg's tex and insert to c2
	bool QueryAndInsert(uint64_t key, float* texcoords, int& tex_id) const;
	bool Exist(uint64_t key) const { return m_all_nodes.find(key) != m_all_nodes.end(); }

	void GetFirstPageTexInfo(int& id, size_t& w, size_t& h) const;
	bool QueryRegion(uint64_t key, int& tex_id, int& xmin, int& ymin, int& xmax, int& ymax) const;

private:
	class Page : boost::noncopyable
	{
	public:
		Page(const ur2::Device& dev, size_t width, size_t height);

		bool AddToTP(size_t width, size_t height, Rect& ret);

		void Clear();

		int GetTexID() const;
		size_t GetWidth() const { return m_width; }
		size_t GetHeight() const { return m_height; }

		void UpdateBitmap(ur2::Context& ctx, const uint32_t* bitmap,
            int width, int height, const Rect& pos, const Rect& dirty_r);
		bool UploadTexture(ur2::Context& ctx);

	private:
		void InitDirtyRect();
		void UpdateDirtyRect(const Rect& r);

	private:
		size_t m_width, m_height;

        ur2::TexturePtr m_tex = nullptr;
		std::unique_ptr<TexPacker> m_tp  = nullptr;

        std::shared_ptr<ur2::WritePixelBuffer> m_pbuf = nullptr;

		Rect m_dirty_rect;

	}; // Page

	struct Node
	{
		uint64_t key = 0;

		size_t page = 0;
		Rect   region;
	};

private:
	int m_width, m_height;
	Callback m_cb;

	std::vector<std::unique_ptr<Page>> m_pages;

	CU_UNORDERED_MAP<uint64_t, Node> m_all_nodes;
	mutable CU_VEC<Node>             m_new_nodes;

}; // CacheGlyph

}

#endif // _DTEX_CACHE_GLYPH_H_