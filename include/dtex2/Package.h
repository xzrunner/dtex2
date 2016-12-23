#ifndef _DTEX_PACKAGE_H_
#define _DTEX_PACKAGE_H_

#include <CU_Uncopyable.h>

#include <vector>

namespace dtex
{

class Texture;

class Package : private cu::Uncopyable
{
public:
	Package(int id);
	
	int GetID() const { return m_id; }

	void AddTexture(Texture* tex) { m_textures.push_back(tex); }
	const std::vector<Texture*>& GetTextures() const { return m_textures; }

	Texture* GetTexture(int idx) const;

private:
	int m_id;

	std::vector<Texture*> m_textures;

	int m_lod_layer;

}; // Package

}

#endif // _DTEX_PACKAGE_H_
