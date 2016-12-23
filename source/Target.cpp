#include "Target.h"
#include "RenderAPI.h"

#include <assert.h>

namespace dtex
{

std::vector<int> Target::m_layers;

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

	assert(RenderAPI::GetTarget() == m_target_id);

	RenderAPI::TargetBindTexture(m_texture_id);
	RenderAPI::CheckTargetStatus();

	m_texture_id = tex_id;
}

void Target::UnbindTexture()
{
	m_texture_id = 0;
}

void Target::Bind()
{
	RenderAPI::Flush();

	assert(!m_layers.empty());
	int curr = m_layers.back();
	if (curr != m_target_id) {
		RenderAPI::SetTarget(m_target_id);
		RenderAPI::TargetBind(m_target_id);
	}

	m_layers.push_back(m_target_id);
}

void Target::Unbind()
{
	assert(!m_layers.size() > 1);

	RenderAPI::Flush();

	int curr = m_layers[m_layers.size() - 1];
	int prev = m_layers[m_layers.size() - 2];
	if (curr != prev) {
		RenderAPI::SetTarget(prev);
		RenderAPI::TargetBind(prev);
	}

	m_layers.pop_back();
}

}