#ifndef _DTEX_CACHE_GLYPH_H_
#define _DTEX_CACHE_GLYPH_H_

#include "Cache.h"
#include "CacheType.h"
#include "Math.h"

#include <CU_Uncopyable.h>

#include <set>
#include <vector>

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

	void Load(uint32_t* bitmap, int width, int height, uint64_t key);
	void Flush();

	void Clear();

private:
	void InitDirtyRect();
	void UpdateDirtyRect(const texpack_pos* pos);

	void UpdateTexture();

private:
	class Node
	{
	public:
		Node(uint64_t key, texpack_pos* pos);

		uint64_t Key() const { return m_key; }
		const Rect& GetRect() const { return m_rect; }

	private:
		uint64_t m_key;
		Rect m_rect;
	}; // Node

private:
	int m_width, m_height;
	Callback m_cb;

	uint32_t* m_buf;

	uint32_t* m_bitmap;
	Texture*  m_tex;
	texpack*  m_tp;

	std::set<uint64_t> m_exists;
	std::vector<Node> m_nodes;

	Rect m_dirty_rect;

}; // CacheGlyph

}

#endif // _DTEX_CACHE_GLYPH_H_