#include "dtex2/CachePkgStatic.h"
#include "dtex2/CP_Texture.h"
#include "dtex2/TextureRaw.h"
#include "dtex2/HardRes.h"
#include "dtex2/Package.h"
#include "dtex2/TextureFormat.h"
#include "dtex2/ResourceAPI.h"
#include "dtex2/RenderAPI.h"
#include "dtex2/DebugDraw.h"
#include "dtex2/CacheAPI.h"
#include "dtex2/LoadResTask.h"

#include <logger.h>
#include <gimg_pvr.h>
#include <gimg_etc2.h>
#include <gimg_import.h>
#include <gimg_typedef.h>

#include <algorithm>

#include <assert.h>
#include <string.h>

namespace dtex
{

static const int MAX_PRELOAD_COUNT = 512;

#if defined(__APPLE__) && !defined(__MACOSX)
	#define LOCAL_TEX_FMT TEXTURE_PVR4
#elif defined(__ANDROID__)
	#define LOCAL_TEX_FMT TEXTURE_ETC2
#else
	#define LOCAL_TEX_FMT TEXTURE_RGBA4
#endif

CachePkgStatic::CachePkgStatic(int tex_size, int tex_fmt)
	: m_remain(0)
	, m_available(false)

{
	int max_sz = HardRes::GetMaxTexSize();
	while (tex_size > max_sz) {
		tex_size = tex_size >> 1;
	}

	m_tex_edge = tex_size;

	m_tex_fmt = (tex_fmt == TEXTURE_INVALID ? LOCAL_TEX_FMT : tex_fmt);

	CP_Texture* tex = new CP_Texture(m_tex_edge, 2, MAX_PRELOAD_COUNT);
	if (tex == nullptr) {
		ResourceAPI::ErrorReload();
	}
	m_textures.push_back(tex);
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

	DebugDraw::Draw(m_textures.back()->GetTexture()->GetID(), 3);

//	DebugDraw::Draw(m_textures[0]->GetTexture()->GetID(), 0);
}

void CachePkgStatic::Clear()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		m_textures[i]->Clear(false);
	}

	m_prenodes.clear();

	m_pkgs.clear();

	m_remain = 0;
}

void CachePkgStatic::Load(const Package* pkg, int lod)
{
	if (m_pkgs.find(pkg->GetID()) != m_pkgs.end()) {
		return;
	}

	auto& textures = pkg->GetTextures();
	for (int i = 0, n = textures.size(); i < n; ++i)
	{
		assert(textures[i]->Type() == TEX_RAW);
		if (m_tex_fmt != TEXTURE_RGBA4 && textures[i]->GetFormat() != m_tex_fmt) {
			continue;
		}
		m_prenodes.push_back(CP_Prenode(pkg, i, lod));
	}

	m_pkgs.insert(pkg->GetID());
}

int CachePkgStatic::LoadFinish(bool async)
{
	if (m_prenodes.empty()) {
		return 0;
	}

	m_prenodes.sort();
	m_prenodes.unique();
	PackPrenodes();
	m_prenodes.clear();

	LoadTexAndRelocateNodes(async);

	return m_textures.size();
}

void CachePkgStatic::PackPrenodes()
{
	static const float SCALE = 1;
	CU_LIST<CP_Prenode>::iterator itr = m_prenodes.begin();
	for ( ; itr != m_prenodes.end(); ++itr)
	{
		const CP_Prenode& prenode = *itr;

		const Package* pkg = prenode.GetPackage();
		int tex_idx = prenode.GetTexIdx();

		const Texture* tex = pkg->GetTexture(tex_idx);
		int w = static_cast<int>(tex->GetWidth() * prenode.GetScale() * SCALE),
			h = static_cast<int>(tex->GetHeight() * prenode.GetScale() * SCALE);
		switch (prenode.GetLod())
		{
		case 1:
			w /= 2;
			h /= 2;
			break;
		case 2:
			w /= 4;
			h /= 4;
			break;
		}
		if (w > m_tex_edge || h > m_tex_edge) {
			LOGW("CachePkgStatic::PackPrenodes tex size too large, w %d, h %d", w, h);
			continue;
		}

		while (true) {
			if (PackPrenode(prenode, SCALE)) {
				break;
			} else {
				CP_Texture* tex = new CP_Texture(m_tex_edge, 2, MAX_PRELOAD_COUNT);
				if (tex == nullptr) {
					ResourceAPI::ErrorReload();
				}
				m_textures.push_back(tex);
			}
		}
	}
}

bool CachePkgStatic::PackPrenode(const CP_Prenode& prenode, float scale)
{
	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		if (m_textures[i]->PackPrenode(prenode, 1, this)) {
			return true;
		}
	}
	return false;
}

void CachePkgStatic::LoadTexAndRelocateNodes(bool async)
{
	int count = 0;
	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		count += m_textures[i]->GetNodes().size();
	}

	CU_VEC<CP_Node*> nodes;
	nodes.reserve(count);

	for (int i = 0, n = m_textures.size(); i < n; ++i) {
		const CU_VEC<CP_Node*>& src = m_textures[i]->GetNodes();
		copy(src.begin(), src.end(), back_inserter(nodes));
	}
	std::sort(nodes.begin(), nodes.end(), NodeTexCmp());

	m_remain = nodes.size();

	if (async)
	{
		for (int i = 0, n = nodes.size(); i < n; ++i) {
			CP_Node* node = nodes[i];

			LoadResTask* task = LoadResTaskMgr::Instance()->Fetch(LoadTextureCB, node);
			ResourceAPI::GetTexFilepath(node->GetSrcPkg()->GetID(), node->GetSrcTexIdx(), node->GetSrcLod(), task->GetResPath());
			ResourceAPI::SubmitTask(task);
		}
	}
	else
	{
		for (int i = 0, n = nodes.size(); i < n; ++i) {
			CP_Node* node = nodes[i];
			ResourceAPI::LoadTexture(node->GetSrcPkg()->GetID(), node->GetSrcTexIdx(), LoadTextureCB, node);
		}
		ResourceAPI::CachePkgStaticTexOk();
		m_available = true;
	}

	CreateTexturesID();
	RelocateNodes();
}

void CachePkgStatic::CreateTexturesID()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i)
	{
		CP_Texture* tex = m_textures[i];
		Texture* tex_impl = tex->GetTexture();
		int w = tex_impl->GetWidth(),
			h = tex_impl->GetHeight();

		int fmt = 0;
		switch (m_tex_fmt)
		{
		case TEXTURE_PVR4:
			fmt = TEXTURE_PVR4;
			break;
		case TEXTURE_ETC2:
			if (RenderAPI::IsSupportETC2()) {
				fmt = TEXTURE_ETC2;
			} else {
				fmt = TEXTURE_RGBA4;
			}
			break;
		case TEXTURE_RGBA4:
			fmt = TEXTURE_RGBA4;
			break;
		default:
			assert(0);
		}

		int id = RenderAPI::CreateTextureID(w, h, fmt);

		tex->SetUD(nullptr);
		tex_impl->SetID(id);
		tex_impl->SetFormat(fmt);
	}
}

void CachePkgStatic::UpdateTextures()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i)
	{
		CP_Texture* tex = m_textures[i];
		Texture* tex_impl = tex->GetTexture();
		int w = tex_impl->GetWidth(),
			h = tex_impl->GetHeight();
		void* pixels = tex->GetUD();
		if (!pixels) {
			return;
		}
		int tex_id = tex_impl->GetID();
		switch (m_tex_fmt)
		{
		case TEXTURE_PVR4:
			LoadTexturePVR4(tex_id, w, h, pixels);
			break;
		case TEXTURE_ETC2:
			LoadTextureETC2(tex_id, w, h, pixels);
			break;
		case TEXTURE_RGBA4:
			RenderAPI::UpdateTexture(pixels, w, h, tex_id);
			break;
		default:
			assert(0);
		}
		free(pixels);
		tex->SetUD(nullptr);
	}
}

void CachePkgStatic::RelocateNodes()
{
	for (int i = 0, n = m_textures.size(); i < n; ++i)
	{
		const CU_VEC<CP_Node*>& src = m_textures[i]->GetNodes();
		for (int j = 0, m = src.size(); j < m; ++j)
		{
			CP_Node* node = src[j];
			const Texture* dst_tex = node->GetDstTex()->GetTexture();
			const Rect& dst_r = node->GetDstRect();
			CacheAPI::RelocatePkg(node->GetSrcPkg()->GetID(), node->GetSrcTexIdx(), node->GetSrcLod(), dst_tex->GetID(),
				dst_tex->GetFormat(), dst_tex->GetWidth(), dst_tex->GetHeight(), dst_r.xmin, dst_r.ymin, dst_r.xmax, dst_r.ymax);
		}
	}
	CacheAPI::RelocatePkgFinish();
}

void CachePkgStatic::LoadTextureCB(int format, int w, int h, const void* data, void* ud)
{
	CP_Node* node = static_cast<CP_Node*>(ud);
	if (!node->GetUD()) {
		return;
	}
	CachePkgStatic* cache = static_cast<CachePkgStatic*>(node->GetUD());
	switch (cache->GetTexFmt())
	{
	case TEXTURE_PVR4:
		assert(format == TEXTURE_PVR4);
		LoadPartPVR4(w, h, data, node);
		break;
	case TEXTURE_ETC2:
		assert(format == TEXTURE_ETC2);
		LoadPartETC2(w, h, data, node);
		break;
	case TEXTURE_RGBA4:
		if (!node->GetDstTex()->GetUD()) {
			uint8_t* ud = BitmapInitBland(node->GetDstTex()->GetTexture()->GetWidth());
			node->GetDstTex()->SetUD(ud);
		}
		switch (format)
		{
		case TEXTURE_RGBA4:
			LoadPartRGBA4(w, h, data, node);
			break;
		case TEXTURE_RGBA8:
			LoadPartRGBA4FromRGBA8(w, h, data, node);
			break;
		case TEXTURE_PVR4:
			LoadPartRGBA4FromPVR4(w, h, data, node);
			break;
		case TEXTURE_ETC2:
			LoadPartRGBA4FromETC2(w, h, data, node);
			break;
		default:
			assert(0);
		}
		break;
	default:
		assert(0);
	}

	if (cache->UpRemain()) {
		cache->UpdateTextures();
		ResourceAPI::CachePkgStaticTexOk();
		cache->SetAvailable();
	}
}

void CachePkgStatic::LoadTextureCB(const void* data, size_t size, void* ud)
{
	ResourceAPI::LoadTexture(data, size, LoadTextureCB, ud);
}

void CachePkgStatic::LoadPartPVR4(int w, int h, const void* data, const CP_Node* node)
{
	assert(IS_POT(w) && IS_POT(h));

	const Rect& dst_pos = node->GetDstRect();
	if (w != dst_pos.xmax - dst_pos.xmin ||
		h != dst_pos.ymax - dst_pos.ymin) {
		return;
	}
	assert(IS_4TIMES(dst_pos.xmin) && IS_4TIMES(dst_pos.ymin));

	CP_Texture* dst_tex = node->GetDstTex();
	int tex_w = dst_tex->GetTexture()->GetWidth(),
		tex_h = dst_tex->GetTexture()->GetHeight();
	if (!dst_tex->GetUD()) {
		void* pixels = gimg_pvr_init_blank(tex_w);
		if (pixels == nullptr) {
			ResourceAPI::ErrorReload();
			return;
		}
		dst_tex->SetUD(pixels);
	}

	int grid_x = dst_pos.xmin >> 2,
		grid_y = dst_pos.ymin >> 2;
	int grid_w = w >> 2,
		grid_h = h >> 2;
	const uint8_t* src_data = (const uint8_t*)(data);
	uint8_t* dst_data = (uint8_t*)(dst_tex->GetUD());
	if (!dst_data) {
		return;
	}
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
	assert(IS_POT(w) && IS_POT(h));

	const Rect& dst_pos = node->GetDstRect();
	if (w != dst_pos.xmax - dst_pos.xmin ||
		h != dst_pos.ymax - dst_pos.ymin) {
		return;
	}
	assert(IS_4TIMES(dst_pos.xmin) && IS_4TIMES(dst_pos.ymin));

	CP_Texture* dst_tex = node->GetDstTex();
	int tex_w = dst_tex->GetTexture()->GetWidth(),
		tex_h = dst_tex->GetTexture()->GetHeight();
	if (!dst_tex->GetUD()) {
		void* pixels = gimg_etc2_init_blank(tex_w);
		if (pixels == nullptr) {
			ResourceAPI::ErrorReload();
			return;
		}
		dst_tex->SetUD(pixels);
	}

	int grid_x = dst_pos.xmin >> 2,
		grid_y = dst_pos.ymin >> 2;
	int grid_w = w >> 2,
		grid_h = h >> 2;
	const uint8_t* src_data = (const uint8_t*)(data);
	uint8_t* dst_data = (uint8_t*)(dst_tex->GetUD());
	if (!dst_data) {
		return;
	}
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

void CachePkgStatic::LoadPartRGBA4(int w, int h, const void* data, const CP_Node* node)
{
	const Rect& dst_pos = node->GetDstRect();
	if (w != dst_pos.xmax - dst_pos.xmin ||
		h != dst_pos.ymax - dst_pos.ymin) {
		return;
	}

	const uint16_t* src_data = (const uint16_t*)(data);
	uint16_t* dst_data = (uint16_t*)(node->GetDstTex()->GetUD());
	if (!dst_data) {
		return;
	}
	const int large_w = node->GetDstTex()->GetTexture()->GetWidth();
	for (int y = 0; y < h; ++y) {
		int idx_src = w * y;
		int idx_dst = dst_pos.xmin + large_w * (dst_pos.ymin + y);
		memcpy(dst_data + idx_dst, src_data + idx_src, 2 * w);
	}
}

void CachePkgStatic::LoadPartRGBA4FromRGBA8(int w, int h, const void* data, const CP_Node* node)
{
	const uint8_t* pixels = static_cast<const uint8_t*>(data);
	uint8_t* rgba4 = gimg_rgba8_to_rgba4_dither(const_cast<uint8_t*>(pixels), w, h);
	if (rgba4 == nullptr) {
		ResourceAPI::ErrorReload();
		return;
	}
	LoadPartRGBA4(w, h, rgba4, node);
	free(rgba4);
}

void CachePkgStatic::LoadPartRGBA4FromPVR4(int w, int h, const void* data, const CP_Node* node)
{
	const uint8_t* pixels = (const uint8_t*)(data);
	if (pixels == nullptr) {
		ResourceAPI::ErrorReload();
		return;
	}
	uint8_t* rgba8 = gimg_pvr_decode_rgba8(pixels, w, h);
	if (rgba8 == nullptr) {
		ResourceAPI::ErrorReload();
		return;
	}
	uint8_t* rgba4 = gimg_rgba8_to_rgba4_dither(rgba8, w, h);
	if (rgba4 == nullptr) {
		ResourceAPI::ErrorReload();
		return;
	}
	gimg_revert_y((uint8_t*)rgba4, w, h, GPF_RGBA4);
	LoadPartRGBA4(w, h, rgba4, node);
	free(rgba4);
	free(rgba8);
}

void CachePkgStatic::LoadPartRGBA4FromETC2(int w, int h, const void* data, const CP_Node* node)
{
	const uint8_t* pixels = (const uint8_t*)(data);
	uint8_t* rgba8 = gimg_etc2_decode_rgba8(pixels, w, h, ETC2PACKAGE_RGBA_NO_MIPMAPS);
	if (rgba8 == nullptr) {
		ResourceAPI::ErrorReload();
		return;
	}
	uint8_t* rgba4 = gimg_rgba8_to_rgba4_dither(rgba8, w, h);
	if (rgba4 == nullptr) {
		ResourceAPI::ErrorReload();
		return;
	}
	LoadPartRGBA4(w, h, rgba4, node);
	free(rgba4);
	free(rgba8);
}

uint8_t* CachePkgStatic::BitmapInitBland(int edge)
{
	size_t sz = edge * edge * 2;
	uint8_t* buf = (uint8_t*)malloc(sz);
	if (buf == nullptr) {
		ResourceAPI::ErrorReload();
		return nullptr;
	}
	memset(buf, 0, sz);
	return buf;
}

void CachePkgStatic::LoadTexturePVR4(int tex_id, int w, int h, const void* data)
{
#if defined( __APPLE__ ) && !defined(__MACOSX)
//	int internal_format = 0;
//	internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	RenderAPI::UpdateTexture(data, w, h, tex_id);
#else
	uint8_t* rgba8 = gimg_pvr_decode_rgba8(static_cast<const uint8_t*>(data), w, h);
	if (rgba8 == nullptr) {
		ResourceAPI::ErrorReload();
		return;
	}
	uint8_t* rgba4 = gimg_rgba8_to_rgba4_dither(rgba8, w, h);
	gimg_revert_y((uint8_t*)rgba4, w, h, GPF_RGBA4);
	RenderAPI::UpdateTexture(rgba4, w, h, tex_id);
	free(rgba4);
	free(rgba8);
#endif
}

void CachePkgStatic::LoadTextureETC2(int tex_id, int w, int h, const void* data)
{
	if (RenderAPI::IsSupportETC2()) {
		RenderAPI::UpdateTexture(data, w, h, tex_id);
	} else {
		uint8_t* rgba8 = gimg_etc2_decode_rgba8(static_cast<const uint8_t*>(data), w, h, ETC2PACKAGE_RGBA_NO_MIPMAPS);
		if (rgba8 == nullptr) {
			ResourceAPI::ErrorReload();
			return;
		}
		uint8_t* rgba4 = gimg_rgba8_to_rgba4_dither(rgba8, w, h);
		if (rgba4 == nullptr) {
			ResourceAPI::ErrorReload();
			return;
		}
		RenderAPI::UpdateTexture(rgba4, w, h, tex_id);
		free(rgba4);
		free(rgba8);
	}
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
