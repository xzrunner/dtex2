#include "dtex2/CP_Prenode.h"
#include "dtex2/Texture.h"
#include "dtex2/Package.h"

#include <stddef.h>
#include <assert.h>

namespace dtex
{

CP_Prenode::CP_Prenode(const Package* pkg, int tex_idx, int lod)
	: m_pkg(pkg)
	, m_tex_idx(tex_idx)
	, m_lod(lod)
	, m_scale(1)
{
}

int CP_Prenode::Area() const
{
	Texture* tex = m_pkg->GetTexture(m_tex_idx);
	assert(tex);
	int w = tex->GetWidth(),
		h = tex->GetHeight();
	switch (m_lod)
	{
	case 0:
		return w * h;
		break;
	case 1:
		return w * h / 4;
		break;
	case 2:
		return w * h / 16;
		break;
	default:
		assert(0);
		return 0;
	}
}

}