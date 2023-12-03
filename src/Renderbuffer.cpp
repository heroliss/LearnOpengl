#include "Renderbuffer.h"
#include "GLCALL.h"

Renderbuffer::Renderbuffer()
{
	GLCALL(glGenRenderbuffers(1, &rbo));
}

Renderbuffer::~Renderbuffer()
{
	//std::cout << "Delete: Renderbuffer " << rbo << std::endl;
	GLCALL(glDeleteRenderbuffers(1, &rbo));
}

void Renderbuffer::Bind() const
{
	GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, rbo));
}

void Renderbuffer::Unbind() const
{
	GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}
