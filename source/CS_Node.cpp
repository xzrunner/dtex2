#include "dtex2/CS_Node.h"
#include "dtex2/Texture.h"

#include <texpack.h>

namespace dtex
{

CS_Node::CS_Node(uint64_t key, const Texture* dst_tex, texpack_pos* dst_pos)
	: m_key(key)
	, m_dst_tex(dst_tex)
	, m_dst_pos(dst_pos)
{
	float xmin = dst_pos->r.xmin * dst_tex->GetWidthInv(),
	      xmax = dst_pos->r.xmax * dst_tex->GetWidthInv(),
	      ymin = dst_pos->r.ymin * dst_tex->GetHeightInv(),
	      ymax = dst_pos->r.ymax * dst_tex->GetHeightInv();
	m_texcoords[0] = xmin; m_texcoords[1] = ymin;
	m_texcoords[2] = xmax; m_texcoords[3] = ymin;
	m_texcoords[4] = xmax; m_texcoords[5] = ymax;
	m_texcoords[6] = xmin; m_texcoords[7] = ymax;
	if (dst_pos->is_rotated)
	{
		float x, y;
		x = m_texcoords[6]; y = m_texcoords[7];
		m_texcoords[6] = m_texcoords[4]; m_texcoords[7] = m_texcoords[5];
		m_texcoords[4] = m_texcoords[2]; m_texcoords[5] = m_texcoords[3];
		m_texcoords[2] = m_texcoords[0]; m_texcoords[3] = m_texcoords[1];
		m_texcoords[0] = x;           m_texcoords[1] = y;
	}
}

}