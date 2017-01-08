#include "CachePkgStatic.h"
#include "CP_Texture.h"
#include "TextureRaw.h"
#include "HardRes.h"
#include "Package.h"
#include "TextureFormat.h"
#include "ResourceAPI.h"
#include "RenderAPI.h"
#include "DebugDraw.h"
#include "CacheAPI.h"
#include "AsyncTask.h"

#include <logger.h>
#include <gimg_pvr.h>
#include <gimg_etc2.h>
#include <gimg_import.h>
#include <gimg_typedef.h>

#include <algorithm>

#include <assert.h>

namespace dtex
{

static const int MAX_PRELOAD_COUNT = 512;

#if defined(__APPLE__) && !defined(__MACOSX)
	#define LOCAL_TEX_FMT TEXTURE_PVR4
#elif defined(__ANDROID__)
	#define LOCAL_TEX_FMT TEXTURE_ETC2
#else
	#define LOCAL_TEX_FMT TEXTURE_RGBA8
#endif

CachePkgStatic::CachePkgStatic(int tex_size, int tex_count)
	: m_remain(0)
{
	int max_sz = HardRes::GetMaxTexSize();
	while (tex_size > max_sz) {
		tex_size = tex_size >> 1;
		tex_count = tex_count << 2;
	}

	m_tex_edge = tex_size;
	m_textures.resize(tex_count);
	for (int i = 0; i < tex_count; ++i) {
		m_textures[i] = new CP_Texture(tex_size, MAX_PRELOAD_COUNT);
	}
}

CachePkgStatic::~CachePkgStatic()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		delete m_textures[i];
	}
}

void CachePkgStatic::DebugDraw() const
{
	if (m_textures.empty()) {
		return;
	}

//	DebugDraw::Draw(m_textures[0]->GetTexture()->GetID(), 3);

	DebugDraw::Draw(m_textures.back()->GetTexture()->GetID(), 3);
}

void CachePkgStatic::Clear()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		m_textures[i]->Clear();
	}
}

void CachePkgStatic::Load(const Package* pkg)
{
	if (m_pkgs.find(pkg->GetID()) != m_pkgs.end()) {
		return;
	}

	const std::vector<Texture*>& textures = pkg->GetTextures();
	for (int i = 0, n = textures.size(); i < n; ++i) 
	{
		assert(textures[i]->Type() == TEX_RAW);
		TextureRaw* tex = static_cast<TextureRaw*>(textures[i]);
		if (LOCAL_TEX_FMT != TEXTURE_RGBA8 && tex->GetFormat() != LOCAL_TEX_FMT) {
			ResourceAPI::LoadTexture(pkg->GetID(), i);
			continue;
		}

		m_prenodes.push_back(CP_Prenode(pkg, i));
	}

	m_pkgs.insert(pkg->GetID());
}

void CachePkgStatic::LoadFinish(bool async)
{
	if (m_prenodes.empty()) {
		return;
	}

	m_prenodes.unique();
	m_prenodes.sort();
	PackPrenodes();
	m_prenodes.clear();

	LoadTexAndRelocateNodes(async);
}

void CachePkgStatic::PackPrenodes()
{
	std::list<CP_Prenode>::iterator itr = m_prenodes.begin();
	for ( ; itr != m_prenodes.end(); ++itr) 
	{
		bool succ = false;
		for (int i = 0, n = m_textures.size(); i < n; ++i) {
			if (m_textures[i]->PackPrenode(*itr, 1, this)) {
				succ = true;
				break;
			}
		}

		if (!succ) {
			LOGW("%s", "dtex_c4_load_end node insert fail.");
			ResourceAPI::LoadTexture(itr->GetPackage()->GetID(), itr->GetTexIdx());
		}
	}
}

void CachePkgStatic::LoadTexAndRelocateNodes(bool async)
{
	int count = 0;
	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		count += m_textures[i]->GetNodes().size();
	}

	std::vector<CP_Node*> nodes;
	nodes.reserve(count);

	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		const std::vector<CP_Node*>& src = m_textures[i]->GetNodes();
		copy(src.begin(), src.end(), back_inserter(nodes));
	}
	std::sort(nodes.begin(), nodes.end(), NodeTexCmp());

	m_remain = nodes.size();

	if (async)
	{
		for (int i = 0, n = nodes.size(); i < n; ++i) {
			CP_Node* node = nodes[i];
			const std::string& filepath = ResourceAPI::GetTexFilepath(node->GetSrcPkg()->GetID(), node->GetSrcTexIdx(), 0);
			AsyncTask::Instance()->Load(filepath, LoadTextureCB, node);
		}
	}
	else
	{
		for (int i = 0, n = nodes.size(); i < n; ++i) {
			CP_Node* node = nodes[i];
			ResourceAPI::LoadTexture(node->GetSrcPkg()->GetID(), node->GetSrcTexIdx(), LoadTextureCB, node);
		}
	}
}

void CachePkgStatic::CreateTextures()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i)
	{
		CP_Texture* tex = m_textures[i];
		Texture* tex_impl = tex->GetTexture();
		int w = tex_impl->GetWidth(),
			h = tex_impl->GetHeight();
		void* pixels = tex->GetUD();
		int id = 0;
		int fmt = 0;
		switch (LOCAL_TEX_FMT)
		{
		case TEXTURE_PVR4:
			id = LoadTexturePVR4(w, h, pixels);
			fmt = TEXTURE_PVR4;
			break;
		case TEXTURE_ETC2:
			id = LoadTextureETC2(w, h, pixels);
			fmt = TEXTURE_ETC2;
			break;
		case TEXTURE_RGBA8:
			id = RenderAPI::CreateTexture(pixels, w, h, TEXTURE_RGBA8);
			fmt = TEXTURE_RGBA8;
			break;
		default:
			assert(0);
		}
		free(pixels);
		tex->SetUD(NULL);
		tex_impl->SetID(id);
		tex_impl->SetFormat(fmt);
	}
}

void CachePkgStatic::RelocateNodes()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i) 
	{
		const std::vector<CP_Node*>& src = m_textures[i]->GetNodes();
		for (int j = 0, m = src.size(); j < m; ++j) 
		{
			CP_Node* node = src[j];
			const Texture* dst_tex = node->GetDstTex()->GetTexture();
			const Rect& dst_r = node->GetDstRect();
			CacheAPI::RelocatePkg(node->GetSrcPkg()->GetID(), node->GetSrcTexIdx(), dst_tex->GetID(), dst_tex->GetFormat(),
				dst_tex->GetWidth(), dst_tex->GetHeight(), dst_r.xmin, dst_r.ymin, dst_r.xmax, dst_r.ymax);
		}
	}
}

void CachePkgStatic::LoadTextureCB(int format, int w, int h, const void* data, void* ud)
{
	CP_Node* node = static_cast<CP_Node*>(ud);
	switch (LOCAL_TEX_FMT)
	{
	case TEXTURE_PVR4:
		assert(format == TEXTURE_PVR4);
		LoadPartPVR4(w, h, data, node);
		break;
	case TEXTURE_ETC2:
		assert(format == TEXTURE_ETC2);
		LoadPartETC2(w, h, data, node);		
		break;
	case TEXTURE_RGBA8:
		if (!node->GetDstTex()->GetUD()) {
			uint8_t* ud = BitmapInitBland(node->GetDstTex()->GetTexture()->GetWidth());
			node->GetDstTex()->SetUD(ud);
		}
		switch (format)
		{
		case TEXTURE_RGBA8:
			LoadPartRGBA8(w, h, data, node);
			break;
		case TEXTURE_PVR4:
			LoadPartRGBA8FromPVR4(w, h, data, node);
			break;
		case TEXTURE_ETC2:
			LoadPartRGBA8FromETC2(w, h, data, node);
			break;
		default:
			assert(0);
		}
		break;
	default:
		assert(0);
	}
	
	CachePkgStatic* c = static_cast<CachePkgStatic*>(node->GetUD());
	if (c->UpRemain()) {
		c->CreateTextures();
		c->RelocateNodes();
	}
}

void CachePkgStatic::LoadTextureCB(const void* data, size_t size, void* ud)
{
	ResourceAPI::LoadTexture(data, size, LoadTextureCB, ud);
}

void CachePkgStatic::LoadPartPVR4(int w, int h, const void* data, const CP_Node* node)
{
	const Rect& dst_pos = node->GetDstRect();
	assert(IS_4TIMES(dst_pos.xmin) && IS_4TIMES(dst_pos.ymin));

	CP_Texture* dst_tex = node->GetDstTex();
	int tex_w = dst_tex->GetTexture()->GetWidth(),
		tex_h = dst_tex->GetTexture()->GetHeight();
	if (!dst_tex->GetUD()) {
		void* pixels = gimg_pvr_init_blank(tex_w);
		dst_tex->SetUD(pixels);
	}

	assert(IS_POT(w) && IS_POT(h)
		&& w == dst_pos.xmax - dst_pos.xmin
		&& h == dst_pos.ymax - dst_pos.ymin);

	int grid_x = dst_pos.xmin >> 2,
		grid_y = dst_pos.ymin >> 2;
	int grid_w = w >> 2,
		grid_h = h >> 2;
	const uint8_t* src_data = (const uint8_t*)(data);
	uint8_t* dst_data = (uint8_t*)(dst_tex->GetUD());
	for (int y = 0; y < grid_h; ++y) 
	{
		for (int x = 0; x < grid_w; ++x) 
		{
			int idx_src = gimg_pvr_get_morton_number(x, y);
			int idx_dst = gimg_pvr_get_morton_number(grid_x + x, grid_y + y);
			assert(idx_dst < tex_w * tex_h / 16);
			int64_t* src = (int64_t*)src_data + idx_src;
			int64_t* dst = (int64_t*)dst_data + idx_dst;
			memcpy(dst, src, sizeof(int64_t));
		}
	}
}

void CachePkgStatic::LoadPartETC2(int w, int h, const void* data, const CP_Node* node)
{
	const Rect& dst_pos = node->GetDstRect();
	assert(IS_4TIMES(dst_pos.xmin) && IS_4TIMES(dst_pos.ymin));

	CP_Texture* dst_tex = node->GetDstTex();
	int tex_w = dst_tex->GetTexture()->GetWidth(),
		tex_h = dst_tex->GetTexture()->GetHeight();
	if (!dst_tex->GetUD()) {
		void* pixels = gimg_etc2_init_blank(tex_w);
		dst_tex->SetUD(pixels);
	}

	assert(IS_POT(w) && IS_POT(h)
		&& w == dst_pos.xmax - dst_pos.xmin
		&& h == dst_pos.ymax - dst_pos.ymin);

	int grid_x = dst_pos.xmin >> 2,
		grid_y = dst_pos.ymin >> 2;
	int grid_w = w >> 2,
		grid_h = h >> 2;
	const uint8_t* src_data = (const uint8_t*)(data);
	uint8_t* dst_data = (uint8_t*)(dst_tex->GetUD());
	const int grid_sz = sizeof(uint8_t) * 8 * 2;
	const int large_grid_w = tex_w >> 2;
	for (int y = 0; y < grid_h; ++y) 
	{
		for (int x = 0; x < grid_w; ++x) 
		{
			int idx_src = x + grid_w * y;
			int idx_dst = grid_x + x + large_grid_w * (grid_y + y);
			assert(idx_dst < tex_w * tex_h / 16);
			memcpy(dst_data + idx_dst * grid_sz, src_data + idx_src * grid_sz, grid_sz);
		}
	}
}

void CachePkgStatic::LoadPartRGBA8(int w, int h, const void* data, const CP_Node* node)
{
	const Rect& dst_pos = node->GetDstRect();
	assert(w == dst_pos.xmax - dst_pos.xmin
		&& h == dst_pos.ymax - dst_pos.ymin);

	const uint8_t* src_data = (const uint8_t*)(data);
	uint8_t* dst_data = (uint8_t*)(node->GetDstTex()->GetUD());
	const int large_w = node->GetDstTex()->GetTexture()->GetWidth();
	for (int y = 0; y < h; ++y) {
		int idx_src = w * y;
		int idx_dst = dst_pos.xmin + large_w * (dst_pos.ymin + y);
		memcpy(dst_data + idx_dst * 4, src_data + idx_src * 4, 4 * w);
	}
}

void CachePkgStatic::LoadPartRGBA8FromPVR4(int w, int h, const void* data, const CP_Node* node)
{
	const uint8_t* pixels = (const uint8_t*)(data);
	uint8_t* uncompressed = gimg_pvr_decode(pixels, w, h);
	gimg_revert_y(uncompressed, w, h, GPF_RGBA);
	LoadPartRGBA8(w, h, uncompressed, node);
	free(uncompressed);
}

void CachePkgStatic::LoadPartRGBA8FromETC2(int w, int h, const void* data, const CP_Node* node)
{
	const uint8_t* pixels = (const uint8_t*)(data);
	uint8_t* uncompressed = gimg_etc2_decode(pixels, w, h, ETC2PACKAGE_RGBA_NO_MIPMAPS);
	LoadPartRGBA8(w, h, uncompressed, node);
	free(uncompressed);
}

uint8_t* CachePkgStatic::BitmapInitBland(int edge)
{
	size_t sz = edge * edge * 4;
	uint8_t* buf = (uint8_t*)malloc(sz);
	memset(buf, 0, sz);
	return buf;
}

int CachePkgStatic::LoadTexturePVR4(int w, int h, const void* data)
{
	int ret = 0;
#if defined( __APPLE__ ) && !defined(__MACOSX)
//	int internal_format = 0;
//	internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	ret = RenderAPI::CreateTexture(data, w, h, TEXTURE_PVR4);
#else
	uint8_t* uncompressed = gimg_pvr_decode(static_cast<const uint8_t*>(data), w, h);
	gimg_revert_y(uncompressed, w, h, GPF_RGBA);
	ret = RenderAPI::CreateTexture(uncompressed, w, h, TEXTURE_RGBA8);
	free(uncompressed);
#endif
	return ret;
}

int CachePkgStatic::LoadTextureETC2(int w, int h, const void* data)
{
	int ret = 0;
#ifdef __ANDROID__
	ret = RenderAPI::CreateTexture(data, w, h, TEXTURE_ETC2);
#else
	uint8_t* uncompressed = gimg_etc2_decode(static_cast<const uint8_t*>(data), w, h, ETC2PACKAGE_RGBA_NO_MIPMAPS);
	ret = RenderAPI::CreateTexture(uncompressed, w, h, TEXTURE_RGBA8);
	free(uncompressed);
#endif // __ANDROID__
	return ret;
}

bool CachePkgStatic::NodeTexCmp::operator () (const CP_Node* n0, const CP_Node* n1) const 
{
	int pkg0 = n0->GetSrcPkg()->GetID(),
		pkg1 = n1->GetSrcPkg()->GetID();
	int tex0 = n0->GetSrcTexIdx(),
		tex1 = n1->GetSrcTexIdx();
	if (pkg0 == pkg1) {
		return tex0 < tex1;
	} else {
		return pkg0 < pkg1;
	}
}

}