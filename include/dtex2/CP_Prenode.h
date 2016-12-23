#ifndef _DTEX_CACHE_PACKAGE_PRENODE_H_
#define _DTEX_CACHE_PACKAGE_PRENODE_H_

namespace dtex
{

class Package;

class CP_Prenode
{
public:
	CP_Prenode(const Package* pkg, int tex_idx);
	// todo
// 	CP_Prenode(const CP_Node& node);
// 	CP_Prenode& operator = (const CP_Node& node);

	bool operator == (const CP_Prenode& node) const;
	bool operator < (const CP_Prenode& node) const;

	const Package* GetPackage() const { return m_pkg; }

	int GetTexIdx() const { return m_tex_idx; }

	float GetScale() const { return m_scale; }

private:
	const Package* m_pkg;
	int   m_tex_idx;
	float m_scale;

}; // CP_Prenode

}

#endif // _DTEX_CACHE_PACKAGE_PRENODE_H_