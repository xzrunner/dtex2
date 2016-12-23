#ifndef _DTEX_RENDER_API_H_
#define _DTEX_RENDER_API_H_

namespace dtex
{

class RenderAPI
{
public:
	struct TextureCB
	{
		int  (*texture_create)(const void* data, int width, int height, int format);
		void (*texture_release)(int id);
		void (*texture_update)(const void* pixels, int w, int h, unsigned int id);
		void (*sub_tex_update)(const void* pixels, int x, int y, int w, int h, unsigned int id);
	};

	struct TargetCB
	{
		int  (*create_target)(int id);
		void (*release_target)(int id);
		void (*target_bind_texture)(int tex_id);
		void (*target_bind)(int id);
		int  (*check_target_status)();
	};

	struct DrawCB
	{
		void (*clear_color)(float r, float g, float b, float a);
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

	struct UtilCB
	{
		bool (*out_of_memory)();
		bool (*is_texture)(unsigned int id);
		void (*check_error)();
	};

	static void InitTexCB(const TextureCB& tex_cb, const TargetCB& target_cb, const DrawCB& draw_cb, const UtilCB& util_cb);

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
	static int  GetTarget();

	static void DrawBegin();
	static void Draw(const float vb[16], int texid);
	//void dtex_shader_draw_triangle(const float* vb, int count);
	static void DrawEnd();

	static void Flush();

	static void ScissorEnable(bool enable);
	static void Scissor(int x, int y, int w, int h);

	static void ViewportPush(int x, int y, int w, int h);
	static void ViewportPop();

	/************************************************************************/
	/* Utility                                                              */
	/************************************************************************/

	static bool OutOfMemory();
	static bool IsTexture(unsigned int id);
	static void CheckError();

}; // RenderAPI

}

#endif // _DTEX_RENDER_API_H_