#ifndef _DTEX_CACHE_API_H_
#define _DTEX_CACHE_API_H_

namespace dtex
{

class CacheAPI
{
public:
	struct Callback
	{
		void (*relocate_pkg)(int src_pkg, int src_tex, int src_lod, int dst_tex_id, int dst_fmt, int dst_w, int dst_h, int dst_xmin, int dst_ymin, int dst_xmax, int dst_ymax);
		void (*relocate_pkg_finish)();
		void (*remove_tex)(int tex_id);

		void (*on_clear_sym_block)(int block_id);
	};

	static void InitCallback(const Callback& cb);

	//////////////////////////////////////////////////////////////////////////

	static void RelocatePkg(int src_pkg, int src_tex, int src_lod, int dst_tex_id, int dst_fmt, int dst_w, int dst_h, int dst_xmin, int dst_ymin, int dst_xmax, int dst_ymax);
	static void RelocatePkgFinish();
	static void RemoveTex(int tex_id);

	static void OnClearSymBlock(int block_id);

}; // CacheAPI

}

#endif // _DTEX_CACHE_API_H_