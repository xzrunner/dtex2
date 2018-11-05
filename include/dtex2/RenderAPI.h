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
		void (*enable_blend)(bool enable);
		void (*draw_begin)();
		void (*draw)(const float vertices[8], const float texcoords[8], int texid);
		void (*draw_end)();
		void (*scissor_push)(int x, int y, int w, int h);
		void (*scissor_pop)();
		void (*scissor_disable)();
		void (*scissor_enable)();
	};

	static void InitCallback(const Callback& draw_cb);

	static void InitRenderContext(ur::RenderContext* rc);
	static ur::RenderContext* GetRenderContext();

	/************************************************************************/
	/* Texture                                                              */
	/************************************************************************/

	static int  CreateTexture(const void* data, int width, int height, int format);
	static int  CreateTextureID(int width, int height, int format);
	static void ReleaseTexture(int id, int width, int height, int format);

	static void UpdateTexture(const void* pixels, int w, int h, unsigned int id);

	/************************************************************************/
	/* Target                                                               */
	/************************************************************************/

	static int  CreateTarget(int id);
	static void ReleaseTarget(int id);

	static void TargetBindTexture(int tex_id);
	static void TargetBind(int id);
	static void TargetUnbind();

	static int  CheckTargetStatus();

	/************************************************************************/
	/* State                                                                */
	/************************************************************************/

	static void SetUnpackRowLength(int len);

	/************************************************************************/
	/* Draw                                                                 */
	/************************************************************************/

	static void ClearColor(float r, float g, float b, float a);
	static void ClearColorPart(float xmin, float ymin, float xmax, float ymax);

	static void SetProgram();

	static void SetTexture(int id);
	static int  GetTexture();

	static void EnableBlend(bool enable);

	static void DrawBegin();
	static void Draw(const float vertices[8], const float texcoords[8], int texid);
	//void dtex_shader_draw_triangle(const float* vb, int count);
	static void DrawEnd();

	static void ScissorPush(int x, int y, int w, int h);
	static void ScissorPop();
	static void ScissorDisable();
	static void ScissorEnable();

	static void GetViewport(int& x, int& y, int& w, int& h);
	static void SetViewport(int x, int y, int w, int h);

	/************************************************************************/
	/* Utility                                                              */
	/************************************************************************/

	static bool OutOfMemory();
	static bool IsTexture(unsigned int id);
	static void CheckError();

	static bool IsSupportETC2();

}; // RenderAPI

}

#endif // _DTEX_RENDER_API_H_