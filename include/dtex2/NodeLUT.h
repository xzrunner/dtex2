#ifndef _DTEX_NODE_LUT_H_
#define _DTEX_NODE_LUT_H_

#include <stdint.h>

#include <vector>

namespace dtex
{

class CS_Node;

class NodeLUT
{
public:
	NodeLUT();
	~NodeLUT();

	void Insert(const CS_Node* node);
	bool Delete(const CS_Node* node);

	const CS_Node* Query(uint32_t key) const;

	void Clear();

private:
	void Rehash() const;

	int Hash(uint32_t key) const;

private:
	mutable std::vector<const CS_Node*>* m_hash;
	mutable int m_hash_sz_idx;

	mutable int m_search_length;
	mutable int m_search_times;

	int m_count;

}; // NodeLUT

}

#endif // _DTEX_NODE_LUT_H_