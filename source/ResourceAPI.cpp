#include "ResourceAPI.h"

namespace dtex
{

static ResourceAPI::Callback CB;

void ResourceAPI::InitCallback(const Callback& cb)
{
	CB = cb;
}

void ResourceAPI::LoadTexture(int pkg_id, int tex_idx)
{
	CB.load_texture(pkg_id, tex_idx);
}

void ResourceAPI::LoadTexture(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud)
{
	CB.load_texture_cb(pkg_id, tex_idx, cb, ud);
}

}