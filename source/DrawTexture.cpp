#include "dtex2/DrawTexture.h"
#include "dtex2/Texture.h"
#include "dtex2/typedef.h"
#include "dtex2/RenderAPI.h"
#include "dtex2/ResCache.h"
#include "dtex2/Target.h"

namespace dtex
{

CU_SINGLETON_DEFINITION(DrawTexture);

DrawTexture::DrawTexture()
	: m_curr(nullptr)
{
}

void DrawTexture::Draw(int src_tex_id, int src_w, int src_h, const Rect& src_r, Texture* dst, const Rect& dst_r, bool rotate)
{
	Bind(dst);

	float vertices[8];
	float w_inv = m_curr->GetWidthInv(),
		  h_inv = m_curr->GetHeightInv();
	float dst_xmin = dst_r.xmin * w_inv * 2 - 1,
		  dst_xmax = dst_r.xmax * w_inv * 2 - 1,
		  dst_ymin = dst_r.ymin * h_inv * 2 - 1,
		  dst_ymax = dst_r.ymax * h_inv * 2 - 1;
	vertices[0] = dst_xmin; vertices[1] = dst_ymin; 
	vertices[2] = dst_xmax; vertices[3] = dst_ymin; 
	vertices[4] = dst_xmax; vertices[5] = dst_ymax; 
	vertices[6] = dst_xmin; vertices[7] = dst_ymax; 
	if (rotate) 
	{
		float x, y;
		x = vertices[6]; y = vertices[7];
		vertices[6] = vertices[4]; vertices[7] = vertices[5];
		vertices[4] = vertices[2]; vertices[5] = vertices[3];
		vertices[2] = vertices[0]; vertices[3] = vertices[1];
		vertices[0] = x;           vertices[1] = y;
	}

	float texcoords[8];
	float src_w_inv = 1.0f / src_w,
		  src_h_inv = 1.0f / src_h;
	float src_xmin = src_r.xmin * src_w_inv,
		  src_xmax = src_r.xmax * src_w_inv,
		  src_ymin = src_r.ymin * src_h_inv,
		  src_ymax = src_r.ymax * src_h_inv;
	texcoords[0] = src_xmin; texcoords[1] = src_ymin; 
	texcoords[2] = src_xmax; texcoords[3] = src_ymin; 
	texcoords[4] = src_xmax; texcoords[5] = src_ymax; 
	texcoords[6] = src_xmin; texcoords[7] = src_ymax; 

	RenderAPI::SetProgram();
	RenderAPI::Draw(vertices, texcoords, src_tex_id);
}

void DrawTexture::ClearTex(Texture* tex, float xmin, float ymin, float xmax, float ymax)
{
	Bind(tex);

	int w = m_curr->GetWidth(),
		h = m_curr->GetHeight();
	RenderAPI::ScissorPush(
		static_cast<int>(w * xmin),
		static_cast<int>(h * ymin),
		static_cast<int>(w * (xmax - xmin)),
		static_cast<int>(h * (ymax - ymin)));
	RenderAPI::ClearColor(0, 0, 0, 0);
	RenderAPI::ScissorPop();
}

void DrawTexture::Flush()
{
	if (m_curr) {
		DrawAfter(m_ctx);
		m_curr = nullptr;
	}
}

void DrawTexture::Clear()
{
	Flush();

	m_curr = nullptr;

	if (m_ctx.target) {
		ResCache::Instance()->ReturnTarget(m_ctx.target);
		m_ctx.target = nullptr;
	}
}

void DrawTexture::ClearAllTex(Texture* tex)
{
	Target* target = ResCache::Instance()->FetchTarget();

	target->Bind();
	target->BindTexture(tex->GetID());

	RenderAPI::ClearColor(0, 0, 0, 0);

	target->UnbindTexture();
	target->Unbind();

	ResCache::Instance()->ReturnTarget(target);
}

void DrawTexture::DrawBefore(Context& ctx)
{
	RenderAPI::DrawBegin();
	RenderAPI::GetViewport(ctx.vx, ctx.vy, ctx.vw, ctx.vh);
	RenderAPI::SetViewport(0, 0, m_curr->GetWidth(), m_curr->GetHeight());

	Target* target = ResCache::Instance()->FetchTarget();
	target->Bind();
	target->BindTexture(m_curr->GetID());

	ctx.target = target;
}

void DrawTexture::DrawAfter(Context& ctx)
{
	ctx.target->UnbindTexture();
	ctx.target->Unbind();
	ResCache::Instance()->ReturnTarget(ctx.target);
	ctx.target = nullptr;

	RenderAPI::SetViewport(ctx.vx, ctx.vy, ctx.vw, ctx.vh);
	RenderAPI::DrawEnd();
}

void DrawTexture::Bind(Texture* tex)
{
	if (!tex || m_curr == tex) {
		return;
	}
	if (m_curr) {
		DrawAfter(m_ctx);
	}
	m_curr = tex;
	DrawBefore(m_ctx);
}

}