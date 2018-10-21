#ifndef _DTEX_PACKAGE_H_
#define _DTEX_PACKAGE_H_

#include "dtex2/Texture.h"

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

	void AddTexture(TexturePtr&& tex) { m_textures.push_back(std::move(tex)); }
	auto& GetTextures() const { return m_textures; }

	Texture* GetTexture(int idx) const;

private:
	int m_id;

	CU_VEC<TexturePtr> m_textures;

	int m_lod_layer;

}; // Package

#ifdef USE_MM_ALLOCATOR
using PackagePtr = std::unique_ptr<Package, mm::alloc_deleter<mm::Allocator<Package>>>;
#else
using PackagePtr = std::unique_ptr<Package>;
#endif // USE_MM_ALLOCATOR

}

#endif // _DTEX_PACKAGE_H_
