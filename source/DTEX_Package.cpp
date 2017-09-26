#include "DTEX_Package.h"
#include "DTEX_Texture.h"

#include <stddef.h>

namespace dtex
{

Package::Package(int id)
	: m_id(id)
{
}

Package::~Package()
{
   for (unsigned long i = 0, n = m_textures.size(); i < n; ++i) {
       delete m_textures[i];
   }
}

Texture* Package::GetTexture(int idx) const
{
	if (idx < 0 || static_cast<size_t>(idx) >= m_textures.size()) {
		return NULL;
	}

	return m_textures[idx];
}

}
