#ifndef _DTEX_RESOURCE_API_H_
#define _DTEX_RESOURCE_API_H_

#include <string>

namespace dtex
{

class Package;

class ResourceAPI
{
public:
	struct Callback
	{
		const std::string& (*get_tex_filepath)(int pkg_id, int tex_idx, int lod_layer);
		void (*load_file)(const std::string& filepath, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud);

		void (*load_texture)(int pkg_id, int tex_idx);
		void (*load_texture_cb)(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);
		void (*load_texture_cb2)(const void* data, size_t size, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);
	};

	static void InitCallback(const Callback& cb);
	
	//////////////////////////////////////////////////////////////////////////

	static const std::string& GetTexFilepath(int pkg_id, int tex_idx, int lod_layer);
	static void LoadFile(const std::string& filepath, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud);

	static void LoadTexture(int pkg_id, int tex_idx);
	static void LoadTexture(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);
	static void LoadTexture(const void* data, size_t size, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);

}; // ResourceAPI

}

#endif // _DTEX_RESOURCE_API_H_