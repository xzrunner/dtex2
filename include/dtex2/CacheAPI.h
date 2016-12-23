#ifndef _DTEX_CACHE_API_H_
#define _DTEX_CACHE_API_H_

namespace dtex
{

class CacheAPI
{
public:
	struct Callback
	{
		void (*relocate_pkg)(int src_pkg, int src_tex, int dst_tex_id, int dst_fmt, int dst_w, int dst_h, int dst_xmin, int dst_ymin, int dst_xmax, int dst_ymax);
	};

	static void InitCallback(const Callback& cb);

	//////////////////////////////////////////////////////////////////////////

	static void RelocatePkg(int src_pkg, int src_tex, int dst_tex_id, int dst_fmt, int dst_w, int dst_h, int dst_xmin, int dst_ymin, int dst_xmax, int dst_ymax);

}; // CacheAPI

}

#endif // _DTEX_CACHE_API_H_