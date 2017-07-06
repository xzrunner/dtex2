#include "TextureRaw.h"
#include "RenderAPI.h"

namespace dtex
{

TextureRaw::TextureRaw(float lod_scale)
	: m_id_alpha(0)
	, m_scale(1)
	, m_lod_scale(lod_scale)
{
}

TextureRaw::~TextureRaw()
{
	if (m_id_alpha != 0) {
		RenderAPI::ReleaseTexture(m_id_alpha, m_width, m_height, m_format);
	}
}

}