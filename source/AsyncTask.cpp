#include "AsyncTask.h"
#include "ResourceAPI.h"

#include <atask_loader.h>

namespace dtex
{

SINGLETON_DEFINITION(AsyncTask);

void load(const char* filepath, void (*unpack)(const void* data, size_t size, void* ud), void* ud)
{
	ResourceAPI::LoadFile(filepath, false, unpack, ud);
}

AsyncTask::AsyncTask()
{
	m_loader = atask_loader_create(load, 4);
}

AsyncTask::~AsyncTask()
{
	atask_loader_release(m_loader);
}

void AsyncTask::Load(const std::string& filepath, void (*parser)(const void* data, size_t size, void* ud), void* ud)
{
	atask_load_file(m_loader, filepath.c_str(), parser, ud, "");
}

void AsyncTask::Update()
{
	atask_loader_update(m_loader);
}

void AsyncTask::Clear()
{
	atask_loader_clear(m_loader);
}

bool AsyncTask::IsEmpty() const
{
	return atask_loader_empty(m_loader);
}

}