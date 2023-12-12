#pragma once

#include <memory>
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;
	unsigned int m_vertexCount;
	unsigned int m_attribeIndex; //��ǰ������������Ϊ0ʱ��ʾ��û��������ӣ���ʱ��һ����ӵ�����������Ϊ��������
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

