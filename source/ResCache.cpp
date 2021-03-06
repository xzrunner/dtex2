#include "dtex2/ResCache.h"
#include "dtex2/Texture.h"
#include "dtex2/Target.h"
#include "dtex2/TextureMid.h"
#include "dtex2/ResourceAPI.h"

#include <assert.h>

namespace dtex
{

CU_SINGLETON_DEFINITION(ResCache);

ResCache::ResCache()
{
}

ResCache::~ResCache()
{
	Clear();
}

void ResCache::Clear()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		delete m_textures[i];
	}
	m_textures.clear();

	for (int i = 0, n = m_targets.size(); i < n; ++i) {
		delete m_targets[i];
	}
	m_targets.clear();
}

Texture* ResCache::FetchTexture(int width, int height)
{
	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		assert(m_textures[i]->Type() == TEX_MID);
		TextureMid* tex = static_cast<TextureMid*>(m_textures[i]);
		if (tex->GetWidth() == width && tex->GetHeight() == height) {
			return tex;
		}
	}
	return nullptr;
}

void ResCache::ReturnTexture(Texture* texture)
{
	if (texture->Type() != TEX_MID) {
		return;
	}

	m_textures.push_back(texture);
}

Target* ResCache::FetchTarget()
{
	if (m_targets.empty()) {
		Target* t = new Target();
		if (t == nullptr) {
			ResourceAPI::ErrorReload();
			return nullptr;
		}
		return t;
	}

	Target* ret = m_targets.back();
	m_targets.pop_back();
	return ret;
}

void ResCache::ReturnTarget(Target* target)
{
	m_targets.push_back(target);
}

}