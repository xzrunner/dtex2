#ifndef _DTEX_PACKAGE_H_
#define _DTEX_PACKAGE_H_

#include "DTEX_Texture.h"

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

namespace dtex
{

class Texture;

class Package : private cu::Uncopyable
{
public:
	Package(int id);
    	
	int GetID() const { return m_id; }

	void AddTexture(TexturePtr& tex) { m_textures.push_back(std::move(tex)); }
	auto& GetTextures() const { return m_textures; }

	Texture* GetTexture(int idx) const;

private:
	int m_id;

	CU_VEC<TexturePtr> m_textures;

	int m_lod_layer;

}; // Package

using PackagePtr = std::unique_ptr<Package, mm::alloc_deleter<mm::Allocator<Package>>>;

}

#endif // _DTEX_PACKAGE_H_
