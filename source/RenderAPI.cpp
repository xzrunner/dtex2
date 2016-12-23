#include "RenderAPI.h"
#include "Statistics.h"

namespace dtex
{

static RenderAPI::TextureCB TEX_CB;
static RenderAPI::TargetCB  TARGET_CB;
static RenderAPI::DrawCB    DRAW_CB;
static RenderAPI::UtilCB    UTIL_CB;

void RenderAPI::InitTexCB(const TextureCB& tex_cb, const TargetCB& target_cb, const DrawCB& draw_cb, const UtilCB& util_cb)
{
	TEX_CB    = tex_cb;
	TARGET_CB = target_cb;
	DRAW_CB   = draw_cb;
	UTIL_CB   = util_cb;
}

/************************************************************************/
/* Texture                                                              */
/************************************************************************/

int RenderAPI::CreateTexture(const void* data, int width, int height, int format)
{
	int ret = TEX_CB.texture_create(data, width, height, format);
	Statistics::Instance()->AddTex(ret, format, width, height);
	return ret;
}

void RenderAPI::ReleaseTexture(int id)
{
	DRAW_CB.set_texture(0);
	TEX_CB.texture_release(id);
	Statistics::Instance()->DeleteTex(id);
}

void RenderAPI::UpdateTexture(const void* pixels, int w, int h, unsigned int id)
{
	TEX_CB.texture_update(pixels, w, h, id);
}

void RenderAPI::UpdateSubTex(const void* pixels, int x, int y, int w, int h, unsigned int id)
{
	TEX_CB.sub_tex_update(pixels, x, y, w, h, id);
}

/************************************************************************/
/* Target                                                               */
/************************************************************************/

int RenderAPI::CreateTarget(int id)
{
	return TARGET_CB.create_target(id);
}

void RenderAPI::ReleaseTarget(int id)
{
	TARGET_CB.release_target(id);
}

void RenderAPI::TargetBindTexture(int tex_id)
{
	TARGET_CB.target_bind_texture(tex_id);
}

void RenderAPI::TargetBind(int id)
{
	TARGET_CB.target_bind(id);
}

int RenderAPI::CheckTargetStatus()
{
	return TARGET_CB.check_target_status();
}

/************************************************************************/
/* Draw                                                                 */
/************************************************************************/

void RenderAPI::ClearColor(float r, float g, float b, float a)
{
	DRAW_CB.clear_color(r, g, b, a);
}

void RenderAPI::ClearColorPart(float xmin, float ymin, float xmax, float ymax)
{
	DRAW_CB.clear_color_part(xmin, ymin, xmax, ymax);
}

void RenderAPI::SetProgram()
{
	DRAW_CB.set_program();
}

void RenderAPI::SetBlend(int mode)
{
	DRAW_CB.set_blend(mode);
}

void RenderAPI::SetTexture(int id)
{
	DRAW_CB.set_texture(id);
}

int RenderAPI::GetTexture()
{
	return DRAW_CB.get_texture();
}

void RenderAPI::SetTarget(int id)
{
	DRAW_CB.set_target(id);
}

int RenderAPI::GetTarget()
{
	return DRAW_CB.get_target();	
}

void RenderAPI::DrawBegin()
{
	DRAW_CB.draw_begin();
}

void RenderAPI::Draw(const float vb[16], int texid)
{
	DRAW_CB.draw(vb, texid);
}

void RenderAPI::DrawEnd()
{
	DRAW_CB.draw_end();
}

void RenderAPI::Flush()
{
	DRAW_CB.draw_flush();
}

void RenderAPI::ScissorEnable(bool enable)
{
	DRAW_CB.scissor_enable(enable);
}

void RenderAPI::Scissor(int x, int y, int w, int h)
{
	DRAW_CB.scissor(x, y, w, h);
}

void RenderAPI::ViewportPush(int x, int y, int w, int h)
{
	DRAW_CB.viewport_push(x, y, w, h);
}

void RenderAPI::ViewportPop()
{
	DRAW_CB.viewport_pop();
}

/************************************************************************/
/* Utility                                                              */
/************************************************************************/

bool RenderAPI::OutOfMemory()
{
	return UTIL_CB.out_of_memory();
}

bool RenderAPI::IsTexture(unsigned int id)
{
	return UTIL_CB.is_texture(id);
}

void RenderAPI::CheckError()
{
	UTIL_CB.check_error();
}

}