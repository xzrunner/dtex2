#ifndef _DTEX_CACHE_SYMBOL_H_
#define _DTEX_CACHE_SYMBOL_H_

#include "dtex2/Cache.h"
#include "dtex2/typedef.h"
#include "dtex2/CacheType.h"
#include "dtex2/NodeLUT.h"
#include "dtex2/CS_Node.h"

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>
#include <unirender2/typedef.h>

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

	virtual int Type() const override { return CACHE_SYMBOL; }
	virtual void DebugDraw() const override;
	virtual void Clear() override;

	void LoadStart();
	void Load(int tex_id, int tex_w, int tex_h, const Rect& r, uint64_t key,
		int padding = 0, int extrude = 0, int src_extrude = 0);
	void LoadFinish();

	const CS_Node* Query(uint64_t key, int& block_id) const;

    // todo
    ur2::TexturePtr GetTexture() const { return nullptr; }

private:
	void ClearBlockData();

	class Block;
	void ClearBlockTex(const Block* b);

private:
	class Prenode
	{
	public:
		Prenode(int tex_id, int tex_w, int tex_h, const Rect& r, uint64_t key, int padding, int extrude, int src_extrude);

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
		int SrcExtrude() const { return m_src_extrude; }

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
		int m_src_extrude;

	}; // Prenode

	struct PrenodeKeyCmp
	{
		bool operator () (const Prenode& lhs, const Prenode& rhs) const {
			return lhs.Key() < rhs.Key();
		}
	};

	class Block : private cu::Uncopyable
	{
	public:
		Block(Texture* tex, int x, int y, int w, int h);
		~Block();

		bool operator == (const Block& block) const {
			return m_x == block.m_x && m_y == block.m_y;
		}
		bool operator < (const Block& block) const {
			return m_x < block.m_x || (m_x == block.m_x && m_y < block.m_y);
		}

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

	class DrawTask
	{
	public:
		DrawTask(Texture* tex, Block* block, const Prenode& pn, const Rect& src, const Rect& dst, bool rotate);

		bool operator == (const DrawTask& node) const { return m_pn->TexID() == node.m_pn->TexID(); }
		bool operator < (const DrawTask& node) const { return m_pn->TexID() < node.m_pn->TexID(); }

		bool Draw() const;

		const Block* GetBlock() const { return m_block; }

	private:
		bool DrawExtrude(int src_tex_id, int src_w, int src_h, const Rect& src_r, const Rect& dst_r, bool rotate, int extrude) const;

	private:
		Texture* m_tex;
		Block*   m_block;

		const Prenode* m_pn;
		Rect m_src, m_dst;
		bool m_rotate;

	}; // DrawTask

private:
	bool InsertNode(const Prenode& node, std::list<DrawTask>& drawlist, std::list<Block*>& clearlist,
		int& clear_block_idx);

private:
	int m_loadable;

	Texture* m_tex;
	Block*   m_blocks[BLOCK_X_SZ * BLOCK_Y_SZ];
	int      m_block_w, m_block_h;

	std::set<Prenode, PrenodeKeyCmp> m_prenodes;
	CU_VEC<CS_Node>    m_nodes;

	int m_clear_block_idx;

}; // CacheSymbol

}

#endif // _DTEX_CACHE_SYMBOL_H_