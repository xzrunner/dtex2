#include "RenderAPI.h"
#include "Statistics.h"

#include <unirender/RenderContext.h>

namespace dtex
{

static RenderAPI::Callback DRAW_CB;

static ur::RenderContext* RC = NULL;

void RenderAPI::InitCallback(const Callback& draw_cb)
{
	DRAW_CB = draw_cb;
}

void RenderAPI::InitRenderContext(ur::RenderContext* rc)
{
	RC = rc;
}

/************************************************************************/
/* Texture                                                              */
/************************************************************************/

int RenderAPI::CreateTexture(const void* data, int width, int height, int format)
{
	int ret = RC->CreateTexture(data, width, height, format);
	Statistics::Instance()->AddTex(ret, format, width, height);
	return ret;
}

void RenderAPI::ReleaseTexture(int id)
{
	RC->BindTexture(id, 0);
	RC->ReleaseTexture(id);
	Statistics::Instance()->DeleteTex(id);
}

void RenderAPI::UpdateTexture(const void* pixels, int w, int h, unsigned int id)
{
	RC->UpdateTexture(pixels, w, h, id);
}

void RenderAPI::UpdateSubTex(const void* pixels, int x, int y, int w, int h, unsigned int id)
{
	RC->UpdateSubTexture(pixels, x, y, w, h, id);
}

/************************************************************************/
/* Target                                                               */
/************************************************************************/

int RenderAPI::CreateTarget(int id)
{
	return RC->CreateRenderTarget(id);
}

void RenderAPI::ReleaseTarget(int id)
{
	RC->ReleaseRenderTarget(id);
}

void RenderAPI::TargetBindTexture(int tex_id)
{
	RC->BindRenderTargetTex(tex_id);
}

void RenderAPI::TargetBind(int id)
{
	RC->BindRenderTarget(id);
}

void RenderAPI::TargetUnbind()
{
	RC->UnbindRenderTarget();
}

int RenderAPI::CheckTargetStatus()
{
	return RC->CheckRenderTargetStatus();
}

/************************************************************************/
/* Draw                                                                 */
/************************************************************************/

void RenderAPI::ClearColor(float r, float g, float b, float a)
{
	RC->SetClearFlag(ur::MASKC);

	int argb = 
		(int)(255 * a) << 24 |
		(int)(255 * r) << 16 |
		(int)(255 * g) <<  8 |
		(int)(255 * b);
	RC->Clear(argb);
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
	RC->BindTexture(id, 0);
}

int RenderAPI::GetTexture()
{
	return RC->GetCurrTexture();
}

void RenderAPI::DrawBegin()
{
	DRAW_CB.draw_begin();
}

void RenderAPI::Draw(const float vertices[8], const float texcoords[8], int texid)
{
	DRAW_CB.draw(vertices, texcoords, texid);
}

void RenderAPI::DrawEnd()
{
	DRAW_CB.draw_end();
}

void RenderAPI::Flush()
{
	DRAW_CB.draw_flush();
}

void RenderAPI::ScissorPush(int x, int y, int w, int h)
{
	DRAW_CB.scissor_push(x, y, w, h);
}

void RenderAPI::ScissorPop()
{
	DRAW_CB.scissor_pop();
}

void RenderAPI::ScissorClose()
{
	DRAW_CB.scissor_close();
}

void RenderAPI::ScissorOpen()
{
	DRAW_CB.scissor_open();
}

void RenderAPI::GetViewport(int& x, int& y, int& w, int& h)
{
	RC->GetViewport(x, y, w, h);
}

void RenderAPI::SetViewport(int x, int y, int w, int h)
{
	RC->SetViewport(x, y, w, h);
}

/************************************************************************/
/* Utility                                                              */
/************************************************************************/

bool RenderAPI::OutOfMemory()
{
	return RC->OutOfMemory();
}

bool RenderAPI::IsTexture(unsigned int id)
{
	return RC->IsTexture(id);
}

void RenderAPI::CheckError()
{
	RC->CheckError();
}

}