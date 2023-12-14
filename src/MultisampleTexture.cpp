#include "MultisampleTexture.h"
#include "GLCALL.h"


MultisampleTexture::MultisampleTexture(unsigned int multisample, unsigned int width, unsigned int height)
	: multisample(multisample), m_id(0), width(width), height(height)
{
	GLCALL(glGenTextures(1, &m_id));
	Bind();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisample, GL_RGB, width, height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	Unbind();
}

MultisampleTexture::~MultisampleTexture()
{
	std::cout << " Release Multisample Texture(" << this->m_id << "): " << std::endl;
	GLCALL(glDeleteTextures(1, &m_id));
}

void MultisampleTexture::Bind() const
{
	GLCALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_id));
}

void MultisampleTexture::Unbind() const
{
	GLCALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
}
