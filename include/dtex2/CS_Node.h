#ifndef _DTEX_CACHE_SYMBOL_NODE_H_
#define _DTEX_CACHE_SYMBOL_NODE_H_

#include <stdint.h>

struct texpack_pos;

namespace dtex
{

class Texture;

class CS_Node
{
public:
	CS_Node(uint64_t key, const Texture* dst_tex, texpack_pos* dst_pos);

	const float* GetTexcoords() const { return m_texcoords; }

	uint64_t Key() const { return m_key; }
	
private:
	uint64_t m_key;

	const Texture* m_dst_tex;
	texpack_pos*   m_dst_pos;

	float m_texcoords[8];

}; // CS_Node

}

#endif // _DTEX_CACHE_SYMBOL_NODE_H_