#include "ResourceAPI.h"

namespace dtex
{

static ResourceAPI::Callback CB;

void ResourceAPI::InitCallback(const Callback& cb)
{
	CB = cb;
}

const std::string& ResourceAPI::GetTexFilepath(int pkg_id, int tex_idx, int lod_layer)
{
	return CB.get_tex_filepath(pkg_id, tex_idx, lod_layer);
}

void ResourceAPI::LoadFile(const std::string& filepath, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud)
{
	CB.load_file(filepath, use_cache, parser_cb, ud);
}

void ResourceAPI::LoadTexture(int pkg_id, int tex_idx)
{
	CB.load_texture(pkg_id, tex_idx);
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