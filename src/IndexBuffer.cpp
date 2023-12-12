#include "IndexBuffer.h"
#include "GLCALL.h"
#include <iostream>

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int size) : m_Count(size / sizeof(data[0])), m_RendererID(0)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));
	GLCALL(glGenBuffers(1, &m_RendererID));
	Bind();
	GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
	Unbind();
}

IndexBuffer::~IndexBuffer()
{
	GLCALL(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const
{
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const
{
	GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
