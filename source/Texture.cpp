#include "Texture.h"
#include "RenderAPI.h"
#include "Target.h"
#include "ResCache.h"

namespace dtex
{
	
Texture::Texture()
	: m_id(0)
	, m_width(0)
	, m_height(0)
	, m_cache_locked(false)
{
}

Texture::~Texture()
{
	if (m_id != 0) {
		RenderAPI::ReleaseTexture(m_id);
	}
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
	int x, y, w, h;
	RenderAPI::GetViewport(x, y, w, h);
	RenderAPI::SetViewport(0, 0, m_width, m_height);

	RenderAPI::Scissor(
		m_width * xmin, 
		m_height * ymin, 
		m_width * (xmax - xmin), 
		m_height * (ymax - ymin));

	Target* target = ResCache::Instance()->FetchTarget();
	target->Bind();
	target->BindTexture(m_id);

	RenderAPI::ScissorEnable(true);
	RenderAPI::ClearColor(0, 0, 0, 0);
	RenderAPI::ScissorEnable(false);

	target->UnbindTexture();
	target->Unbind();
	ResCache::Instance()->ReturnTarget(target);

	RenderAPI::SetViewport(x, y, w, h);
}

}