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
	CacheGlyph(int width, int height);
	virtual ~CacheGlyph();

	virtual int Type() const { return CACHE_GLYPH; }
	virtual void DebugDraw() const;

	void Load(uint32_t* bitmap, int width, int height, uint32_t key);
	void Flush();

	void Clear();

private:
	void InitDirtyRect();

	void UpdateTexture();

private:
	class Node
	{
	public:
		Node(uint32_t key, texpack_pos* pos) 
			: m_key(key), m_pos(pos) {}

	private:
		uint32_t m_key;

		texpack_pos* m_pos;

	}; // Node

	class NodeCmp
	{
	public:
		bool operator () (const Node& n0, const Node& n1) const;
	}; // NodeCmp

private:
	uint32_t* m_buf;

	int m_width, m_height;

	uint32_t* m_bitmap;
	Texture*  m_tex;
	texpack*  m_tp;

	std::set<uint32_t> m_exists;
	std::vector<Node> m_nodes;

	Rect m_dirty_rect;

}; // CacheGlyph

}

#endif // _DTEX_CACHE_GLYPH_H_