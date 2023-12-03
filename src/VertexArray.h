#pragma once

#include <memory>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;
	unsigned int m_vertexCount;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const std::shared_ptr<VertexBuffer>& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void UnBind() const;
	unsigned int inline GetVertexCount() const { return m_vertexCount; }

	std::shared_ptr<VertexBuffer> vb;
};

