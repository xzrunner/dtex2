#ifndef _DTEX_TARGET_H_
#define _DTEX_TARGET_H_

#include <cu/uncopyable.h>

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