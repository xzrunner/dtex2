#ifndef _DTEX_CACHE_GLYPH_H_
#define _DTEX_CACHE_GLYPH_H_

#include "dtex2/Cache.h"
#include "dtex2/CacheType.h"
#include "dtex2/typedef.h"

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <memory>

#include <boost/noncopyable.hpp>

#include <stdint.h>

namespace ur { class PixelBuffer; }

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
	CacheGlyph(int width, int height, const Callback& cb);

	virtual int Type() const override { return CACHE_GLYPH; }
	virtual void DebugDraw() const override;
	virtual void Clear() override;

	void Load(const uint32_t* bitmap, int width, int height, uint64_t key);
	void Flush();

	// query from cg's tex and insert to c2
	bool QueryAndInsert(uint64_t key, float* texcoords, int& tex_id) const;
	bool Exist(uint64_t key) const { return m_all_nodes.find(key) != m_all_nodes.end(); }

private:
	class Page : boost::noncopyable
	{
	public:
		Page(size_t width, size_t height);

		bool AddToTP(size_t width, size_t height, Rect& ret);

		void Clear();

		int GetTexID() const;

		void UpdateBitmap(const uint32_t* bitmap, int width, int height,
			const Rect& pos, const Rect& dirty_r);
		void UploadTexture();

	private:
		void InitDirtyRect();
		void UpdateDirtyRect(const Rect& r);

	private:
		size_t m_width, m_height;

		std::unique_ptr<Texture>   m_tex = nullptr;
		std::unique_ptr<TexPacker> m_tp  = nullptr;

		std::unique_ptr<ur::PixelBuffer> m_pbuf = nullptr;

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