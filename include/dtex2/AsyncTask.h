#ifndef _DTEX_ASYNC_TASK_H_
#define _DTEX_ASYNC_TASK_H_

#include <CU_Singleton.h>

#include <string>

struct tasks_loader;

namespace dtex
{

class AsyncTask
{
public:
	void Load(const std::string& filepath, 
		      void (*parser)(const void* data, size_t size, void* ud), 
			  void (*release)(void* ud),
			  void* ud);

	void Update();

	void Clear();

	bool IsEmpty() const;

private:
	tasks_loader* m_loader;
	
	SINGLETON_DECLARATION(AsyncTask);

}; // AsyncTask

}

#endif // _DTEX_ASYNC_TASK_H_