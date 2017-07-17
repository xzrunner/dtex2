#include "NodeLUT.h"
#include "ResourceAPI.h"

#include <logger.h>

namespace dtex
{

static const size_t HASH_SZ_TBL[] = {
	11, 23, 47, 97, 197, 397, 797, 1597, 3203, 6421, 12853, 25717, 51439, 102881, 202881, 402881
};

static const int HASH_SZ_TBL_SZ = sizeof(HASH_SZ_TBL) / sizeof(HASH_SZ_TBL[0]);

static const int MAX_AVERAGE_SEARCH_LENGTH = 3;

NodeLUT::NodeLUT()
	: m_search_length(0)
	, m_search_times(0)
	, m_count(0)
{
	m_hash_sz_idx = 0;
	m_hash = new std::vector<Node>[HASH_SZ_TBL[m_hash_sz_idx]];
	if (m_hash == NULL) {
		ResourceAPI::ErrorReload();
	}
}

NodeLUT::~NodeLUT()
{
	delete[] m_hash;
}

void NodeLUT::Insert(const Node& node)
{
	int idx = Hash(node.key);
	m_hash[idx].push_back(node);
	++m_count;
}

bool NodeLUT::Delete(const Node& node)
{
	if ((float)m_search_length / m_search_times > MAX_AVERAGE_SEARCH_LENGTH) {
		Rehash();
	}

	++m_search_times;
	int idx = Hash(node.key);
	std::vector<Node>& list = m_hash[idx];
	for (int i = 0, n = list.size(); i < n; ++i) 
	{
		++m_search_length;
		if (list[i].val == node.val) {
			list.erase(list.begin() + i);
			--m_count;
			return true;
		}
	}
	return false;
}

int NodeLUT::Query(uint64_t key) const
{
	if ((float)m_search_length / m_search_times > MAX_AVERAGE_SEARCH_LENGTH) {
		Rehash();
	}

	++m_search_times;
	int idx = Hash(key);
	const std::vector<Node>& list = m_hash[idx];
	for (int i = 0, n = list.size(); i < n; ++i) 
	{
		++m_search_length;
		if (list[i].key == key) {
			return list[i].val;
		}
	}
	return -1;
}

void NodeLUT::Clear()
{
	m_hash_sz_idx = 0;

	delete[] m_hash;
	m_hash = new std::vector<Node>[HASH_SZ_TBL[m_hash_sz_idx]];	
	if (m_hash == NULL) {
		ResourceAPI::ErrorReload();
	}

	m_search_length = m_search_times = 0;

	m_count = 0;
}

void NodeLUT::Rehash() const
{
	if (m_hash_sz_idx + 1 >= HASH_SZ_TBL_SZ) {
		LOGW("NodeLUT::Rehash error hash sz idx %d, %f\n", m_hash_sz_idx, (float)m_search_length / m_search_times);
		return;
	}

	++m_hash_sz_idx;
	std::vector<Node>* new_hash = new std::vector<Node>[HASH_SZ_TBL[m_hash_sz_idx]];
	if (new_hash == NULL) {
		ResourceAPI::ErrorReload();
		return;
	}
	for (int i = 0, n = HASH_SZ_TBL[m_hash_sz_idx - 1]; i < n; ++i) {
		const std::vector<Node>& list = m_hash[i];
		for (int j = 0, m = list.size(); j < m; ++j) {
			int idx = Hash(list[j].key);
			new_hash[idx].push_back(list[j]);	
		}
	}

	delete[] m_hash;
	m_hash = new_hash;

	m_search_length = m_search_times = 0;
}

int NodeLUT::Hash(uint64_t key) const
{
	return key % HASH_SZ_TBL[m_hash_sz_idx];
}

}