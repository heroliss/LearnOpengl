#include "VertexBuffer.h"
#include "GLCALL.h"

VertexBuffer::VertexBuffer(const void* data, unsigned long long size, unsigned int count, bool dynamicDraw) : m_Count(count), m_RendererID(0)
{
	GLCALL(glGenBuffers(1, &m_RendererID));
	Bind();
	GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	Unbind();
}

VertexBuffer::~VertexBuffer()
{
	std::cout << "Delete VertexBuffer: " << m_RendererID << std::endl;
	GLCALL(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::SetData(const void* data, const long size, const long offset) const
{
	Bind();
	GLCALL(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
	Unbind();
}
