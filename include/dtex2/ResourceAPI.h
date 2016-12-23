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
		void (*load_texture)(int pkg_id, int tex_idx);
		void (*load_texture_cb)(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);
	};

	static void InitCallback(const Callback& cb);
	
	//////////////////////////////////////////////////////////////////////////

	static void LoadTexture(int pkg_id, int tex_idx);
	static void LoadTexture(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud);

}; // ResourceAPI

}

#endif // _DTEX_RESOURCE_API_H_