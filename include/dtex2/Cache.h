#ifndef _DTEX_CACHE_H_
#define _DTEX_CACHE_H_

namespace dtex
{

class Cache
{
public:
	virtual ~Cache() {}
	virtual int Type() const = 0;
	virtual void DebugDraw() const = 0;
	
}; // Cache

}

#endif // _DTEX_CACHE_H_