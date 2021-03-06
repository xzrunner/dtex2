#include "dtex2/Texture.h"
#include "dtex2/RenderAPI.h"
#include "dtex2/CacheAPI.h"

namespace dtex
{

Texture::Texture()
	: m_id(0)
	, m_width(0)
	, m_height(0)
	, m_width_inv(0)
	, m_height_inv(0)
	, m_cache_locked(false)
{
}

Texture::~Texture()
{
	if (m_id != 0)
	{
		RenderAPI::ReleaseTexture(m_id, m_width, m_height, m_format);
		CacheAPI::RemoveTex(m_id);
	}
}

void Texture::SetSize(int w, int h)
{
	m_width = w;
	m_height = h;
	m_width_inv = 1.0f / w;
	m_height_inv = 1.0f / h;
}

}