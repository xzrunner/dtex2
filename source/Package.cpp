#include "dtex2/Package.h"
#include "dtex2/Texture.h"

#include <stddef.h>

namespace dtex
{

Package::Package(int id)
	: m_id(id)
{
}

Texture* Package::GetTexture(int idx) const
{
	if (idx < 0 || static_cast<size_t>(idx) >= m_textures.size()) {
		return NULL;
	}

	return m_textures[idx].get();
}

}
