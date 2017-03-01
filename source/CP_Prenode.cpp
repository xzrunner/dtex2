#include "CP_Prenode.h"
#include "Texture.h"
#include "DTEX_Package.h"

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

int CP_Prenode::Area() const
{
	Texture* tex = m_pkg->GetTexture(m_tex_idx);
	assert(tex);
	return tex->GetWidth() * tex->GetHeight();
}

}