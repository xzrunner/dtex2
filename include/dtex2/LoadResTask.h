#ifndef _DTEX_LOAD_RES_TASK_H_
#define _DTEX_LOAD_RES_TASK_H_

#include <CU_Singleton.h>
#include <multitask/Task.h>

namespace dtex
{

class LoadResTask : public mt::Task
{
public:
	LoadResTask(void (*finish_cb)(const void* data, size_t size, void* ud), void* ud);
	virtual ~LoadResTask();

	virtual void Run();

	void Flush();

	void OnLoad(const void* data, size_t size);

	char* GetResPath() { return m_res_path; }

	void Init(void (*finish_cb)(const void* data, size_t size, void* ud), void* ud);
	void Release();

private:
	char m_res_path[512];

	void*  m_data;
	size_t m_size;

	void (*m_on_finish)(const void* data, size_t size, void* ud);
	void* m_ud;

}; // LoadResTask

class LoadResTaskMgr
{
public:
	LoadResTask* Fetch(void (*finish_cb)(const void* data, size_t size, void* ud), void* ud);

	void AddResult(LoadResTask* task);

	bool IsEmpty() { return m_count == 0; }

	void Flush();

private:
	int m_count;

	mt::TaskQueue m_freelist;
	mt::SafeTaskQueue m_result;

	SINGLETON_DECLARATION(LoadResTaskMgr)

}; // LoadResTaskMgr

}

#endif // _DTEX_LOAD_RES_TASK_H_