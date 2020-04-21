#include "dtex2/RenderAPI.h"
#include "dtex2/ResourceAPI.h"

#include <unirender2/Device.h>
#include <unirender2/Context.h>

namespace dtex
{

static RenderAPI::Callback DRAW_CB;

static ur2::Context* UR_CTX = nullptr;
static ur2::Device*  UR_DEV = nullptr;

void RenderAPI::InitCallback(const Callback& draw_cb)
{
	DRAW_CB = draw_cb;
}

void RenderAPI::InitRenderContext(ur2::Context* ur_ctx)
{
    UR_CTX = ur_ctx;
}

ur2::Context* RenderAPI::GetRenderContext()
{
	return UR_CTX;
}

/************************************************************************/
/* Texture                                                              */
/************************************************************************/

int RenderAPI::CreateTexture(const void* data, int width, int height, int format)
{
	//ResourceAPI::StatTexAdd(width, height, format);

	//int ret = RC->CreateTexture(data, width, height, format);
	//return ret;

    return 0;
}

int RenderAPI::CreateTextureID(int width, int height, int format)
{
	//ResourceAPI::StatTexAdd(width, height, format);

	//int ret = RC->CreateTextureID(width, height, format);
	//return ret;

    return 0;
}

void RenderAPI::ReleaseTexture(int id, int width, int height, int format)
{
	//ResourceAPI::StatTexRemove(width, height, format);

	//RC->ReleaseTexture(id);
}

void RenderAPI::UpdateTexture(const void* pixels, int w, int h, unsigned int id)
{
	//RC->UpdateTexture(id, pixels, w, h);
}

/************************************************************************/
/* Target                                                               */
/************************************************************************/

int RenderAPI::CreateTarget(int id)
{
	//return RC->CreateRenderTarget(id);
    return 0;
}

void RenderAPI::ReleaseTarget(int id)
{
	//RC->ReleaseRenderTarget(id);
}

void RenderAPI::TargetBindTexture(int tex_id)
{
	//RC->BindRenderTargetTex(tex_id);
}

void RenderAPI::TargetBind(int id)
{
	//RC->BindRenderTarget(id);
}

void RenderAPI::TargetUnbind()
{
	//RC->UnbindRenderTarget();
}

int RenderAPI::CheckTargetStatus()
{
	//return RC->CheckRenderTargetStatus();
    return 0;
}

/************************************************************************/
/* State                                                                */
/************************************************************************/

void RenderAPI::SetUnpackRowLength(int len)
{
    UR_CTX->SetUnpackRowLength(len);
}

/************************************************************************/
/* Draw                                                                 */
/************************************************************************/

void RenderAPI::ClearColor(float r, float g, float b, float a)
{
	//RC->SetClearFlag(ur::MASKC);

	//int argb =
	//	(int)(255 * a) << 24 |
	//	(int)(255 * r) << 16 |
	//	(int)(255 * g) <<  8 |
	//	(int)(255 * b);
	//RC->SetClearColor(argb);
	//RC->Clear();
}

void RenderAPI::ClearColorPart(float xmin, float ymin, float xmax, float ymax)
{
	//DRAW_CB.clear_color_part(xmin, ymin, xmax, ymax);
}

void RenderAPI::SetProgram()
{
	//DRAW_CB.set_program();
}

void RenderAPI::SetTexture(int id)
{
	//RC->BindTexture(id, 0);
}

int RenderAPI::GetTexture()
{
	//return RC->GetCurrTexture();
    return 0;
}

void RenderAPI::EnableBlend(bool enable)
{
	DRAW_CB.enable_blend(enable);
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

void RenderAPI::ScissorPush(int x, int y, int w, int h)
{
	DRAW_CB.scissor_push(x, y, w, h);
}

void RenderAPI::ScissorPop()
{
	DRAW_CB.scissor_pop();
}

void RenderAPI::ScissorDisable()
{
	DRAW_CB.scissor_disable();
}

void RenderAPI::ScissorEnable()
{
	DRAW_CB.scissor_enable();
}

void RenderAPI::GetViewport(int& x, int& y, int& w, int& h)
{
	//RC->GetViewport(x, y, w, h);
}

void RenderAPI::SetViewport(int x, int y, int w, int h)
{
	//RC->SetViewport(x, y, w, h);
}

/************************************************************************/
/* Utility                                                              */
/************************************************************************/

bool RenderAPI::OutOfMemory()
{
	//return RC->OutOfMemory();
    return false;
}

bool RenderAPI::IsTexture(unsigned int id)
{
	//return RC->IsTexture(id);
    return false;
}

void RenderAPI::CheckError()
{
	//RC->CheckError();
}

bool RenderAPI::IsSupportETC2()
{
	//return RC->IsSupportETC2();
    return false;
}

}