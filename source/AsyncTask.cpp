#include <stdlib.h>
#include "AsyncTask.h"
#include "ResourceAPI.h"

#include <tasks_loader.h>

#define THREAD_NUM  (2)

namespace dtex
{

SINGLETON_DEFINITION(AsyncTask);

void load(const char* filepath, void (*unpack)(const void* data, size_t size, void* ud), void* ud)
{
	ResourceAPI::LoadFile(filepath, false, unpack, ud);
}

AsyncTask::AsyncTask()
{
	m_loader = tasks_loader_create(THREAD_NUM);
}

AsyncTask::~AsyncTask()
{
	tasks_loader_release(m_loader);
	free(m_loader);
	m_loader = NULL;
}

void AsyncTask::Load(const std::string& filepath, 
					 void (*parser)(const void* data, size_t size, void* ud), 
					 void (*release)(void* ud),
					 void* ud)
{
	struct tasks_load_cb params;
	params.load = load;
	params.parser    = parser;
	params.release   = release;
	params.parser_ud = ud;
	tasks_load_file(m_loader, filepath.c_str(), &params, "");
}

void AsyncTask::Update()
{
	tasks_loader_update(m_loader);
}

void AsyncTask::Clear()
{
	// tasks_loader_clear(m_loader);
	tasks_loader_release(m_loader);
	free(m_loader);
	m_loader = tasks_loader_create(THREAD_NUM);
}

bool AsyncTask::IsEmpty() const
{
	return tasks_loader_empty(m_loader);
}

}