#ifndef _DTEX_TEXTURE_RAW_H_
#define _DTEX_TEXTURE_RAW_H_

#include "Texture.h"

namespace dtex
{

class TextureRaw : public Texture
{
public:
	TextureRaw(float lod_scale);
	virtual ~TextureRaw();
	
	virtual TEXTURE_TYPE Type() const { return TEX_RAW; }

private:
	// for etc1
	unsigned int m_id_alpha;

	float m_scale;
	float m_lod_scale;

}; // TextureRaw

}

#endif // _DTEX_TEXTURE_RAW_H_