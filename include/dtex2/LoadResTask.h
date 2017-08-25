#ifndef _DTEX_LOAD_RES_TASK_H_
#define _DTEX_LOAD_RES_TASK_H_

#include <multitask/Task.h>

namespace dtex
{

class LoadResTask : public mt::Task
{
public:
	LoadResTask(void (*finish_cb)(const void* data, size_t size, void* ud), void* ud);
	virtual ~LoadResTask();

	virtual void Run();
	virtual bool Finish();

	void OnLoad(const void* data, size_t size);

	char* GetResPath() { return m_res_path; }

	static bool IsTaskEmpty() { return m_count == 0; }

private:
	static const unsigned int TASK_TYPE = 3;

private:
	char m_res_path[512];

	void*  m_data;
	size_t m_size;

	void (*m_on_finish)(const void* data, size_t size, void* ud);
	void* m_ud;

	static int m_count;

}; // LoadResTask

}

#endif // _DTEX_LOAD_RES_TASK_H_