#include "CP_Prenode.h"
#include "Texture.h"
#include "Package.h"

#include <stddef.h>
#include <assert.h>

namespace dtex
{

CP_Prenode::CP_Prenode(const Package* pkg, int tex_idx)
	: m_pkg(pkg)
	, m_tex_idx(tex_idx)
	, m_scale(1)
{
}

bool CP_Prenode::operator == (const CP_Prenode& node) const
{
	return m_pkg == node.m_pkg && m_tex_idx == node.m_tex_idx;
}

bool CP_Prenode::operator < (const CP_Prenode& node) const
{
	Texture* t0 = m_pkg->GetTexture(m_tex_idx);
	Texture* t1 = node.m_pkg->GetTexture(node.m_tex_idx);
	assert(t0 && t1);
	return t0->GetWidth() * t0->GetHeight() 
		 > t1->GetWidth() * t1->GetHeight();
}

}