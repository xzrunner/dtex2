#ifndef _DTEX_RESOURCE_API_H_
#define _DTEX_RESOURCE_API_H_

#include <cstddef>

namespace mt { class Task; }

namespace dtex
{

class Package;

class ResourceAPI
{
public:
	struct Callback
	{
		void (*error_reload)();

		void (*get_tex_filepath)(int pkg_id, int tex_idx, int lod_layer, char* res_path);
		void (*load_file)(const void* res_path, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud);

		void (*load_texture)(int pkg_id, int tex_idx, int lod);
		void (*load_texture_cb)(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);
		void (*load_texture_cb2)(const void* data, size_t size, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);

		void (*cache_pkg_static_tex_ok)();

		void (*submit_task)(mt::Task* task);

		void (*stat_tex_add)(int width, int height, int format);
		void (*stat_tex_remove)(int width, int height, int format);
	};

	static void InitCallback(const Callback& cb);

	//////////////////////////////////////////////////////////////////////////

	static void ErrorReload();

	static void GetTexFilepath(int pkg_id, int tex_idx, int lod_layer, char* buf);
	static void LoadFile(const void* res_path, bool use_cache, void (*load_cb)(const void* data, size_t size, void* ud), void* ud);

	static void LoadTexture(int pkg_id, int tex_idx, int lod);
	static void LoadTexture(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);
	static void LoadTexture(const void* data, size_t size, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);

	static void CachePkgStaticTexOk();

	static void SubmitTask(mt::Task* task);

	static void StatTexAdd(int width, int height, int format);
	static void StatTexRemove(int width, int height, int format);

}; // ResourceAPI

}

#endif // _DTEX_RESOURCE_API_H_