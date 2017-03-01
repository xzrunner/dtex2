#include "DTEX_Statistics.h"
#include "DTEX_TextureFormat.h"

#include <iostream>

#include <assert.h>

namespace dtex
{

SINGLETON_DEFINITION(Statistics);

Statistics::Statistics()
{
}

Statistics::~Statistics()
{
}

void Statistics::AddTex(int id, int type, int width, int height)
{
	assert(m_textures.find(id) == m_textures.end());

	Texture tex;
	tex.id   = id;
	tex.type = type;
	tex.w    = width;
	tex.h    = height;

	m_textures.insert(std::make_pair(id, tex));
}

void Statistics::DeleteTex(int id)
{
	std::map<int, Texture>::iterator itr = m_textures.find(id);
	assert(itr != m_textures.end());

	m_textures.erase(itr);
}

int Statistics::GetTexTotSize() const
{
	int sz = 0;
	std::map<int, Texture>::const_iterator itr = m_textures.begin();
	for ( ; itr != m_textures.end(); ++itr) {
		const Texture& tex = itr->second;
		sz += GetTexSize(tex.type, tex.w, tex.h);
	}
	return sz;
}

void Statistics::Print() const
{
	std::map<int, Texture>::const_iterator itr = m_textures.begin();
	for ( ; itr != m_textures.end(); ++itr) {
		const Texture& tex = itr->second;
		std::cout << "tex: " << tex.type << " " << tex.w << " " << tex.h << '\n';
	}
}

int Statistics::GetTexSize(int format, int width, int height)
{
	switch (format)
	{
	case TEXTURE_RGBA8 :
		return width * height * 4;
	case TEXTURE_RGBA4 :
	case TEXTURE_PVR2 :
		return width * height / 4;
	case TEXTURE_PVR4 :
	case TEXTURE_ETC1 :
		return width * height / 2;
	case TEXTURE_ETC2:
		return width * height;
	default:
		return 0;
	}
}

}