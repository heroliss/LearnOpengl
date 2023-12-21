#include "UniformBuffer.h"
#include "GLCALL.h"

UniformBuffer::UniformBuffer(const void* data, unsigned long long int size, bool dynamicDraw) : m_RendererID(0)
{
	GLCALL(glGenBuffers(1, &m_RendererID));
	Bind();
	GLCALL(glBufferData(GL_UNIFORM_BUFFER, size, data, dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
	Unbind();
}

UniformBuffer::~UniformBuffer()
{
	std::cout << "Delete UniformBuffer: " << m_RendererID << std::endl;
	GLCALL(glDeleteBuffers(1, &m_RendererID));
}

void UniformBuffer::Bind() const
{
	GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID));
}

void UniformBuffer::Unbind() const
{
	GLCALL(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::SetData(const void* data, const long size, const long offset) const
{
	Bind();
	GLCALL(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
	Unbind();
}

void UniformBuffer::BindPoint(unsigned int bindingPoint, unsigned long long int size, unsigned long long int startOffset)
{
	if (size == 0)
	{
		GLCALL(glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID));
	}
	else
	{
		GLCALL(glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID, startOffset, size));
	}
}
