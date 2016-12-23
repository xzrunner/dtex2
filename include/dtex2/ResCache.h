#ifndef _DTEX_RES_CACHE_H_
#define _DTEX_RES_CACHE_H_

#include <CU_Singleton.h>

#include <vector>

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
	std::vector<Texture*> m_textures;
	std::vector<Target*>  m_targets;
	
	SINGLETON_DECLARATION(ResCache);

}; // ResCache

}

#endif // _DTEX_RES_CACHE_H_