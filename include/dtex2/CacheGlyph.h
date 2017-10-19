#ifndef _DTEX_CACHE_GLYPH_H_
#define _DTEX_CACHE_GLYPH_H_

#include "Cache.h"
#include "CacheType.h"
#include "DTEX_Math.h"

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <stdint.h>

struct texpack;
struct texpack_pos;

namespace dtex
{

class Texture;

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
	virtual ~CacheGlyph();

	virtual int Type() const { return CACHE_GLYPH; }
	virtual void DebugDraw() const;
	virtual void Clear();

	void Load(uint32_t* bitmap, int width, int height, uint64_t key);
	void Flush();

	// query from cg's tex and insert to c2
	bool QueryAndInsert(uint64_t key, float* texcoords, int& tex_id) const;
	bool Exist(uint64_t key) const { return m_all_nodes.find(key) != m_all_nodes.end(); }
	
private:
	void InitDirtyRect();
	void UpdateDirtyRect(const texpack_pos* pos);

	void UpdateTexture();

private:
	int m_width, m_height;
	Callback m_cb;

	uint32_t* m_buf;

	uint32_t* m_bitmap;
	Texture*  m_tex;
	texpack*  m_tp;

	CU_UNORDERED_MAP<uint64_t, Rect> m_all_nodes;
	mutable CU_VEC<std::pair<uint64_t, Rect>> m_new_nodes;

	Rect m_dirty_rect;

}; // CacheGlyph

}

#endif // _DTEX_CACHE_GLYPH_H_