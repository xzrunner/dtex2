#ifndef _DTEX_CACHE_SYMBOL_H_
#define _DTEX_CACHE_SYMBOL_H_

#include "Cache.h"
#include "Math.h"
#include "CacheType.h"
#include "NodeLUT.h"
#include "CS_Node.h"

#include <CU_Uncopyable.h>

#include <list>

#include <stdint.h>

struct texpack;
struct texpack_pos;

namespace dtex
{

class Texture;

class CacheSymbol : public Cache, private cu::Uncopyable
{
public:
	CacheSymbol(int width, int height);
	virtual ~CacheSymbol();

	virtual int Type() const { return CACHE_SYMBOL; }
	virtual void DebugDraw() const;

	void Clear();
	
	void LoadStart();
	void Load(int tex_id, int tex_w, int tex_h, const Rect& r, uint64_t key, 
		int padding = 0, int extrude = 0);
	void LoadFinish();

	const CS_Node* Query(uint64_t key) const;

	int GetTexID() const;

private:
	void ClearBlock();

private:
	class Prenode
	{
	public:
		Prenode(int tex_id, int tex_w, int tex_h, const Rect& r, uint64_t key, int padding, int extrude);

		bool operator == (const Prenode& node) const { return m_key == node.m_key; }
		bool operator < (const Prenode& node) const { return MaxEdge() > node.MaxEdge(); }

		int TexID() const { return m_tex_id; }
		int TexWidth() const { return m_tex_w; }
		int TexHeight() const { return m_tex_h; }

		int Width() const { return m_rect.xmax - m_rect.xmin; }
		int Height() const { return m_rect.ymax - m_rect.ymin; }

		uint64_t Key() const { return m_key; }

		const Rect& GetRect() const { return m_rect; }

		int Padding() const { return m_padding; }
		int Extrude() const { return m_extrude; }

	private:
		int Area() const { return Width() * Height(); }
		int MaxEdge() const { 
			int w = Width(), h = Height();
			return w >= h ? w : h;
		}

	private:
		int m_tex_id;
		int m_tex_w, m_tex_h;
		Rect m_rect;

		uint64_t m_key;

		int m_padding, m_extrude;

	}; // Prenode

	class Block : private cu::Uncopyable
	{
	public:
		Block(Texture* tex, int x, int y, int w, int h);
		~Block();

		void Clear();

		int Query(uint64_t key) const;

		texpack_pos* Insert(const Prenode& node, int extend);
		void Insert(uint64_t key, int val);
		
		int OffX() const { return m_x; }
		int OffY() const { return m_y; }

	private:
		Texture* m_tex;

		int m_x, m_y;

		texpack* m_tp;
		NodeLUT  m_lut;

	}; // Block

	static const int BLOCK_X_SZ = 2;
	static const int BLOCK_Y_SZ = 2;

private:
	bool InsertNode(const Prenode& node);

	void DrawExtrude(int src_tex_id, int src_w, int src_h, const Rect& src_r, const Rect& dst_r, bool rotate, int extrude) const;

private:
	int m_loadable;
	
	Texture* m_tex;
	Block*   m_blocks[BLOCK_X_SZ * BLOCK_Y_SZ];
	int      m_block_w, m_block_h;

	std::list<Prenode>    m_prenodes;
	std::vector<CS_Node>  m_nodes;

	int m_clear_block_idx;

}; // CacheSymbol

}

#endif // _DTEX_CACHE_SYMBOL_H_