#ifndef _DTEX_STATISTICS_H_
#define _DTEX_STATISTICS_H_

#include <CU_Singleton.h>

#include <map>

namespace dtex
{

class Statistics
{
public:
	void AddTex(int id, int type, int width, int height);
	void DeleteTex(int id);
	
	int  GetTexTotSize() const;

	void Print() const;

private:
	static int GetTexSize(int format, int width, int height);

private:
	struct Texture
	{
		int id;
		int type;
		int w, h;
	};

private:
	std::map<int, Texture> m_textures;

	SINGLETON_DECLARATION(Statistics);

}; // Statistics

}

#endif // _DTEX_STATISTICS_H_