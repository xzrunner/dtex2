#ifndef _DTEX_TEXTURE_H_
#define _DTEX_TEXTURE_H_

#include "TextureType.h"

#include <CU_Uncopyable.h>

namespace dtex
{

class Texture : private cu::Uncopyable
{
public:
	Texture();
	virtual ~Texture();

	virtual TEXTURE_TYPE Type() const = 0;

	int  GetWidth() const { return m_width; }
	int  GetHeight() const { return m_height; }
	void SetSize(int w, int h) { m_width = w; m_height = h; }

	void SetID(int id) { m_id = id; }
	int  GetID() const { return m_id; }

	int  GetFormat() const { return m_format; }
	void SetFormat(int format) { m_format = format; }

	void Clear();
	void Clear(float xmin, float ymin, float xmax, float ymax);

protected:
	unsigned int m_id;
	int m_format;

	int m_width, m_height;
	
	bool m_cache_locked;

}; // Texture

}

#endif // _DTEX_TEXTURE_H_