#ifndef _DTEX_NODE_LUT_H_
#define _DTEX_NODE_LUT_H_

#include <cu/cu_stl.h>

#include <stdint.h>

namespace dtex
{

class NodeLUT
{
public:
	struct Node
	{
		Node(uint64_t key, int val) 
			: key(key), val(val) {}

		uint64_t key;
		int      val;
	};

public:
	NodeLUT();
	~NodeLUT();

	void Insert(const Node& node);
	bool Delete(const Node& node);

	int Query(uint64_t key) const;

	void Clear();

private:
	void Rehash() const;

	int Hash(uint64_t key) const;

private:
	mutable CU_VEC<Node>* m_hash;
	mutable int m_hash_sz_idx;

	mutable int m_search_length;
	mutable int m_search_times;

	int m_count;

}; // NodeLUT

}

#endif // _DTEX_NODE_LUT_H_