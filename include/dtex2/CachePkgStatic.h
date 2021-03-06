#ifndef _DTEX_CACHE_PACKAGE_STATIC_H_
#define _DTEX_CACHE_PACKAGE_STATIC_H_

#include "dtex2/CP_Prenode.h"
#include "dtex2/Cache.h"
#include "dtex2/CacheType.h"

#include <cu/uncopyable.h>
#include <cu/cu_stl.h>

#include <stdint.h>
#include <stddef.h>

namespace dtex
{

class Package;
class CP_Texture;
class CP_Node;

class CachePkgStatic : public Cache, private cu::Uncopyable
{
public:
	CachePkgStatic(int tex_size, int tex_fmt);
	virtual ~CachePkgStatic();

	virtual int Type() const override { return CACHE_PKG_STATIC; }
	virtual void DebugDraw() const override;
	virtual void Clear() override;

	bool IsEmpty() const { return m_pkgs.empty(); }

	void Load(const Package* pkg, int lod);
	int LoadFinish(bool async = false);

	bool UpRemain() { return --m_remain == 0; }

	int GetTexSize() const { return m_tex_edge; }
	int GetTexFmt() const { return m_tex_fmt; }

	bool IsAvailable() const { return m_available; }
	void SetAvailable() { m_available = true; }

private:
	void PackPrenodes();
	bool PackPrenode(const CP_Prenode& prenode, float scale);
	void LoadTexAndRelocateNodes(bool async);

	void CreateTexturesID();
	void UpdateTextures();

	void RelocateNodes();

	static void LoadTextureCB(int format, int w, int h, const void* data, void* ud);
	static void LoadTextureCB(const void* data, size_t size, void* ud);

	static void LoadPartPVR4(int w, int h, const void* data, const CP_Node* node);
	static void LoadPartETC2(int w, int h, const void* data, const CP_Node* node);
	static void LoadPartRGBA4(int w, int h, const void* data, const CP_Node* node);

	static void LoadPartRGBA4FromRGBA8(int w, int h, const void* data, const CP_Node* node);
	static void LoadPartRGBA4FromPVR4(int w, int h, const void* data, const CP_Node* node);
	static void LoadPartRGBA4FromETC2(int w, int h, const void* data, const CP_Node* node);

	static uint8_t* BitmapInitBland(int edge);

	static void LoadTexturePVR4(int tex_id, int w, int h, const void* data);
	static void LoadTextureETC2(int tex_id, int w, int h, const void* data);

private:
	class NodeTexCmp
	{
	public:
		bool operator () (const CP_Node* n0, const CP_Node* n1) const;
	}; // NodeTexCmp

private:
	int m_tex_edge;
	int m_tex_fmt;

	CU_VEC<CP_Texture*> m_textures;

	std::list<CP_Prenode> m_prenodes;

	CU_SET<int> m_pkgs;

	int m_remain;

	bool m_available;

}; // CachePkgStatic

}

#endif // _DTEX_CACHE_PACKAGE_STATIC_H_