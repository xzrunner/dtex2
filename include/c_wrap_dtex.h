#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _dtex_wrap_c_h_
#define _dtex_wrap_c_h_

#include <stdbool.h>

void* dtex_query_cache(const char* key);

bool  dtex_loading_task_empty();

/************************************************************************/
/* cache pkg static                                                     */
/************************************************************************/

enum DTEX_TEXTURE_FORMAT {
	DTEX_TEXTURE_INVALID = 0,
	DTEX_TEXTURE_RGBA8,
	DTEX_TEXTURE_RGBA4,
	DTEX_TEXTURE_RGB,
	DTEX_TEXTURE_RGB565,
	DTEX_TEXTURE_A8,
    DTEX_TEXTURE_RED,
    DTEX_TEXTURE_R16,
	DTEX_TEXTURE_DEPTH,	// use for render target
	DTEX_TEXTURE_PVR2,
	DTEX_TEXTURE_PVR4,
	DTEX_TEXTURE_ETC1,
	DTEX_TEXTURE_ETC2,
};

void* dtex_cache_pkg_static_query(const char* key);
void* dtex_cache_pkg_static_create(const char* key, int tex_size, int tex_fmt);
void  dtex_cache_pkg_static_delete(const char* key);

int   dtex_cache_pkg_static_load(void* cache, int pkg_id, int lod);
int   dtex_cache_pkg_static_load_finish(void* cache, bool async);

bool  dtex_cache_pkg_static_is_empty(const void* cache);

bool  dtex_cache_pkg_static_is_available(const void* cache);

#endif // _dtex_wrap_c_h_

#ifdef __cplusplus
}
#endif