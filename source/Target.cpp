#include "dtex2/Target.h"
#include "dtex2/RenderAPI.h"

#include <assert.h>

namespace dtex
{

Target::Target()
	: m_texture_id(0)
{
	m_target_id = RenderAPI::CreateTarget(0);
}

Target::~Target()
{
	RenderAPI::ReleaseTarget(m_target_id);
}

void Target::BindTexture(int tex_id)
{
	if (m_texture_id == tex_id) {
		return;
	}

	RenderAPI::TargetBindTexture(tex_id);
	RenderAPI::CheckTargetStatus();

	m_texture_id = tex_id;
}

void Target::UnbindTexture()
{
	m_texture_id = 0;
}

void Target::Bind()
{
	RenderAPI::TargetBind(m_target_id);
}

void Target::Unbind()
{
	RenderAPI::TargetUnbind();
}

}