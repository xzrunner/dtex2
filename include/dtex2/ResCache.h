#ifndef _DTEX_RES_CACHE_H_
#define _DTEX_RES_CACHE_H_

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>

namespace dtex
{

class Texture;
class Target;

class ResCache
{
public:
	void Clear();

	Texture* FetchTexture(int width, int height);
	void ReturnTexture(Texture* texture);

	Target* FetchTarget();
	void ReturnTarget(Target* target);

private:
	CU_VEC<Texture*> m_textures;
	CU_VEC<Target*>  m_targets;

	CU_SINGLETON_DECLARATION(ResCache);

}; // ResCache

}

#endif // _DTEX_RES_CACHE_H_