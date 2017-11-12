#ifndef _DTEX_TEXTURE_MID_H_
#define _DTEX_TEXTURE_MID_H_

#include "dtex2/Texture.h"

namespace dtex
{

class TextureMid : public Texture
{
public:
	TextureMid(int width, int height, int bpp, bool init_pixels);
	virtual ~TextureMid();
	
	virtual TEXTURE_TYPE Type() const override { return TEX_MID; }
	virtual size_t Size() const override { return sizeof(TextureMid); }

private:
	int InitPixels(int width, int height, int bpp);

}; // TextureMid

}

#endif // _DTEX_TEXTURE_MID_H_