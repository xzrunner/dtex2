#ifndef _DTEX_DRAW_TEXTURE_H_
#define _DTEX_DRAW_TEXTURE_H_

#include <cu/cu_macro.h>

namespace dtex
{

struct Rect;
class Texture;
class Target;

class DrawTexture
{
public:
	void Draw(int src_tex_id, int src_w, int src_h, const Rect& src_r, 
		Texture* dst, const Rect& dst_r, bool rotate);

	void ClearTex(Texture* tex, float xmin, float ymin, float xmax, float ymax);

	void Flush();

	void Clear();

	static void ClearAllTex(Texture* tex);

private:
	struct Context
	{
		int vx, vy, vw, vh;
		Target* target;

		Context() : vx(0), vy(0), vw(0), vh(0), target(NULL) {}
	};

private:
	void DrawBefore(Context& ctx);
	void DrawAfter(Context& ctx);

	void Bind(Texture* tex);

private:
	Texture* m_curr;
	Context m_ctx;

	CU_SINGLETON_DECLARATION(DrawTexture);

}; // DrawTexture

}

#endif // _DTEX_DRAW_TEXTURE_H_