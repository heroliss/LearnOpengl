#pragma once

#include <memory>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;
	unsigned int m_vertexCount;
	unsigned int m_attribeIndex; //当前的属性索引，为0时表示还没有属性添加，此时第一次添加的属性组数视为顶点数量
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const std::shared_ptr<VertexBuffer>& vb, const VertexBufferLayout& layout);

	void PopVertexBuffer();

	void Bind() const;
	void UnBind() const;
	unsigned int inline GetVertexCount() const { return m_vertexCount; }
	std::vector<std::shared_ptr<VertexBuffer>> vbList;
	std::vector<int> attributeCountList;
};

