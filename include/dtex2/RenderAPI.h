#ifndef _DTEX_RENDER_API_H_
#define _DTEX_RENDER_API_H_

namespace ur { class RenderContext; }

namespace dtex
{

class RenderAPI
{
public:
	struct Callback
	{
		void (*clear_color_part)(float xmin, float ymin, float xmax, float ymax);
		void (*set_program)();
		void (*set_blend)(int mode);
		void (*set_texture)(int id);
		int  (*get_texture)();
		void (*set_target)(int id);
		int  (*get_target)();
		void (*draw_begin)();
		void (*draw)(const float vb[16], int texid);
		void (*draw_end)();
		void (*draw_flush)();
		void (*scissor_enable)(bool enable);
		void (*scissor)(int x, int y, int w, int h);
		void (*viewport_push)(int x, int y, int w, int h);
		void (*viewport_pop)();
	};

	static void InitCallback(const Callback& draw_cb);

	static void InitRenderContext(ur::RenderContext* rc);

	/************************************************************************/
	/* Texture                                                              */
	/************************************************************************/

	static int  CreateTexture(const void* data, int width, int height, int format);
	static void ReleaseTexture(int id);

	static void UpdateTexture(const void* pixels, int w, int h, unsigned int id);
	static void UpdateSubTex(const void* pixels, int x, int y, int w, int h, unsigned int id);

	/************************************************************************/
	/* Target                                                               */
	/************************************************************************/

	static int  CreateTarget(int id);
	static void ReleaseTarget(int id);

	static void TargetBindTexture(int tex_id);
	static void TargetBind(int id);

	static int  CheckTargetStatus();

	/************************************************************************/
	/* Draw                                                                 */
	/************************************************************************/

	static void ClearColor(float r, float g, float b, float a);
	static void ClearColorPart(float xmin, float ymin, float xmax, float ymax);

	static void SetProgram();
	static void SetBlend(int mode);

	static void SetTexture(int id);
	static int  GetTexture();

	static void SetTarget(int id);

	static void DrawBegin();
	static void Draw(const float vb[16], int texid);
	//void dtex_shader_draw_triangle(const float* vb, int count);
	static void DrawEnd();

	static void Flush();

	static void ScissorEnable(bool enable);
	static void Scissor(int x, int y, int w, int h);

	static void GetViewport(int& x, int& y, int& w, int& h);
	static void SetViewport(int x, int y, int w, int h);

	/************************************************************************/
	/* Utility                                                              */
	/************************************************************************/

	static bool OutOfMemory();
	static bool IsTexture(unsigned int id);
	static void CheckError();

}; // RenderAPI

}

#endif // _DTEX_RENDER_API_H_