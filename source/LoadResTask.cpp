#include "LoadResTask.h"
#include "ResourceAPI.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

namespace dtex
{

/************************************************************************/
/* class LoadResTask                                                    */
/************************************************************************/

LoadResTask::LoadResTask(void (*finish_cb)(const void* data, size_t size, void* ud), void* ud)
	: mt::Task(TASK_TYPE)
	, m_on_finish(finish_cb)
	, m_ud(ud)
{
	memset(m_res_path, 0, sizeof(m_res_path));

	m_data = NULL;
	m_size = 0;
}

LoadResTask::~LoadResTask()
{
	free(m_data);
}

static inline void 
on_load(const void* data, size_t size, void* ud)
{
	LoadResTask* task = static_cast<LoadResTask*>(ud);
	task->OnLoad(data, size);
}

void LoadResTask::Run()
{
	LoadResTaskMgr::Instance()->AddResult(this);

	ResourceAPI::LoadFile(m_res_path, false, on_load, this);
}

void LoadResTask::Flush()
{
	if (m_data && m_size != 0) {
		m_on_finish(m_data, m_size, m_ud);
	}
}

void LoadResTask::OnLoad(const void* data, size_t size)
{
	if (m_data) {
		free(m_data); m_data = NULL;
	}
	m_size = size;
	m_data = malloc(m_size);
	if (m_data) {
		memcpy(m_data, data, m_size);
	} else {
		m_size = 0;
	}
}

void LoadResTask::Init(void (*finish_cb)(const void* data, size_t size, void* ud), void* ud)
{
	assert(!m_data);
	memset(m_res_path, 0, sizeof(m_res_path));
	m_on_finish = finish_cb;
	m_ud = ud;
}

void LoadResTask::Release()
{
	free(m_data); m_data = NULL;
	m_size = 0;
}

/************************************************************************/
/* class LoadResTaskMgr                                                 */
/************************************************************************/

SINGLETON_DEFINITION(LoadResTaskMgr)

LoadResTaskMgr::LoadResTaskMgr()
	: m_count(0)
{	
}

LoadResTask* LoadResTaskMgr::Fetch(void (*finish_cb)(const void* data, size_t size, void* ud), void* ud)
{
	++m_count;
	mt::Task* t = m_freelist.Front();
	LoadResTask* tt = static_cast<LoadResTask*>(t);	
	if (!tt) {
		tt = new LoadResTask(finish_cb, ud);
	} else {
		m_freelist.Pop();
		tt->Init(finish_cb, ud);
	}
	return tt;
}

void LoadResTaskMgr::AddResult(LoadResTask* task)
{
	m_result.Push(task);
}

void LoadResTaskMgr::Flush()
{
	while (mt::Task* t = m_result.TryPop())
	{
		LoadResTask* tt = static_cast<LoadResTask*>(t);
		tt->Flush();
		tt->Release();
		m_freelist.Push(t);
		--m_count;
	}
}

}