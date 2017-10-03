#ifndef _DTEX_TEXTURE_H_
#define _DTEX_TEXTURE_H_

#include "TextureType.h"

#include <cu/uncopyable.h>

namespace dtex
{

class Texture : private cu::Uncopyable
{
public:
	Texture();
	virtual ~Texture();

	virtual TEXTURE_TYPE Type() const = 0;

	int   GetWidth() const { return m_width; }
	int   GetHeight() const { return m_height; }
	float GetWidthInv() const { return m_width_inv; }
	float GetHeightInv() const { return m_height_inv; }
	void  SetSize(int w, int h);

	void SetID(int id) { m_id = id; }
	int  GetID() const { return m_id; }

	int  GetFormat() const { return m_format; }
	void SetFormat(int format) { m_format = format; }

protected:
	unsigned int m_id;
	int m_format;

	int m_width, m_height;
	float m_width_inv, m_height_inv;
	
	bool m_cache_locked;

}; // Texture

}

#endif // _DTEX_TEXTURE_H_