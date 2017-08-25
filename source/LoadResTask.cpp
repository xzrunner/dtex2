#include "LoadResTask.h"
#include "ResourceAPI.h"

namespace dtex
{

int LoadResTask::m_count = 0;

LoadResTask::LoadResTask(void (*finish_cb)(const void* data, size_t size, void* ud), void* ud)
	: mt::Task(TASK_TYPE, true)
	, m_data(NULL)
	, m_size(0)
	, m_on_finish(finish_cb)
	, m_ud(ud)
{
	++m_count;
	memset(m_res_path, 0, sizeof(m_res_path));
}

LoadResTask::~LoadResTask()
{
	--m_count;
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
	ResourceAPI::LoadFile(m_res_path, false, on_load, this);
}

bool LoadResTask::Finish()
{
	if (m_data && m_size != 0) {
		m_on_finish(m_data, m_size, m_ud);
	}
	return true;
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

}