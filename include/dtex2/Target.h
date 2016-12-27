#ifndef _DTEX_TARGET_H_
#define _DTEX_TARGET_H_

#include <CU_Uncopyable.h>

#include <vector>

namespace dtex
{

class Target : private cu::Uncopyable
{
public:
	Target();
	~Target();

	void BindTexture(int tex_id);
	void UnbindTexture();

	void Bind();
	void Unbind();

private:
	int m_target_id;
	int m_texture_id;

}; // Target

}

#endif // _DTEX_TARGET_H_