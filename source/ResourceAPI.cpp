#include "ResourceAPI.h"

namespace dtex
{

static ResourceAPI::Callback CB;

void ResourceAPI::InitCallback(const Callback& cb)
{
	CB = cb;
}

void ResourceAPI::GetTexFilepath(int pkg_id, int tex_idx, int lod_layer, char* res_path)
{
	CB.get_tex_filepath(pkg_id, tex_idx, lod_layer, res_path);
}

void ResourceAPI::LoadFile(const void* res_path, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud)
{
	CB.load_file(res_path, use_cache, parser_cb, ud);
}

void ResourceAPI::LoadTexture(int pkg_id, int tex_idx, int lod)
{
	CB.load_texture(pkg_id, tex_idx, lod);
}

void ResourceAPI::LoadTexture(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud)
{
	CB.load_texture_cb(pkg_id, tex_idx, cb, ud);
}

void ResourceAPI::LoadTexture(const void* data, size_t size, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud)
{
	CB.load_texture_cb2(data, size, cb, ud);
}

}