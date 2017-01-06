#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _dtex_wrap_c_h_
#define _dtex_wrap_c_h_

#include <stdbool.h>

void* dtex_query_cache(const char* key);

void* dtex_create_cache_pkg_static(const char* key, int tex_size, int tex_count);
void  dtex_cache_pkg_static_load(void* cache, int pkg_id);
void  dtex_cache_pkg_static_load_finish(void* cache);

bool  dtex_loading_task_empty();

#endif // _dtex_wrap_c_h_

#ifdef __cplusplus
}
#endif