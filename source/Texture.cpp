#include "Texture.h"
#include "RenderAPI.h"
#include "Target.h"
#include "ResCache.h"
#include "Math.h"

namespace dtex
{
	
Texture::Texture()
	: m_id(0)
	, m_width(0)
	, m_height(0)
	, m_width_inv(0)
	, m_height_inv(0)
	, m_cache_locked(false)
{
}

Texture::~Texture()
{
	if (m_id != 0) {
		RenderAPI::ReleaseTexture(m_id);
	}
}

void Texture::SetSize(int w, int h) 
{ 
	m_width = w; 
	m_height = h; 
	m_width_inv = 1.0f / w;
	m_height_inv = 1.0f / h;
}

void Texture::Clear()
{
	Target* target = ResCache::Instance()->FetchTarget();

	target->Bind();
	target->BindTexture(m_id);

	RenderAPI::ClearColor(0, 0, 0, 0);

	target->UnbindTexture();
	target->Unbind();

	ResCache::Instance()->ReturnTarget(target);
}

void Texture::Clear(float xmin, float ymin, float xmax, float ymax)
{
	int vx, vy, vw, vh;
	Target* target = DrawBefore(vx, vy, vw, vh);

	RenderAPI::Scissor(
		m_width * xmin, 
		m_height * ymin, 
		m_width * (xmax - xmin), 
		m_height * (ymax - ymin));

	RenderAPI::ScissorEnable(true);
	RenderAPI::ClearColor(0, 0, 0, 0);
	RenderAPI::ScissorEnable(false);

	DrawAfter(target, vx, vy, vw, vh);
}

void Texture::DrawFrom(int src_tex_id, int src_w, int src_h, const Rect& src_r, const Rect& dst_r, bool rotate)
{
	int vx, vy, vw, vh;
	Target* target = DrawBefore(vx, vy, vw, vh);

	float vertices[8];
	float dst_xmin = dst_r.xmin * m_width_inv * 2 - 1,
		  dst_xmax = dst_r.xmax * m_width_inv * 2 - 1,
		  dst_ymin = dst_r.ymin * m_height_inv * 2 - 1,
		  dst_ymax = dst_r.ymax * m_height_inv * 2 - 1;
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

	DrawAfter(target, vx, vy, vw, vh);
}

Target* Texture::DrawBefore(int& vx, int& vy, int& vw, int& vh)
{
	RenderAPI::DrawBegin();
	RenderAPI::GetViewport(vx, vy, vw, vh);
	RenderAPI::SetViewport(0, 0, m_width, m_height);

	Target* target = ResCache::Instance()->FetchTarget();
	target->Bind();
	target->BindTexture(m_id);

	return target;
}

void Texture::DrawAfter(Target* target, int vx, int vy, int vw, int vh)
{
	target->UnbindTexture();
	target->Unbind();
	ResCache::Instance()->ReturnTarget(target);

	RenderAPI::SetViewport(vx, vy, vw, vh);
	RenderAPI::DrawEnd();
}

}