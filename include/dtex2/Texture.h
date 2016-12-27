#ifndef _DTEX_TEXTURE_H_
#define _DTEX_TEXTURE_H_

#include "TextureType.h"

#include <CU_Uncopyable.h>

namespace dtex
{

class Target;
struct Rect;

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

	void Clear();
	void Clear(float xmin, float ymin, float xmax, float ymax);

	void DrawFrom(int src_tex_id, int src_w, int src_h, const Rect& src_r, const Rect& dst_r, bool rotate);

private:
	Target* DrawBefore(int& vx, int& vy, int& vw, int& vh);
	void DrawAfter(Target* target, int vx, int vy, int vw, int vh);

protected:
	unsigned int m_id;
	int m_format;

	int m_width, m_height;
	float m_width_inv, m_height_inv;
	
	bool m_cache_locked;

}; // Texture

}

#endif // _DTEX_TEXTURE_H_