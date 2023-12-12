#include "VertexArray.h"

VertexArray::VertexArray() : m_vertexCount(0), m_RendererID(0), m_attribeIndex(0)
{
	GLCALL(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCALL(glDeleteVertexArrays(1, &m_RendererID));
}

/// <summary>
/// 第一次添加计算顶点数量，后续再添加视为追加属性
/// </summary>
/// <param name="vb"></param>
/// <param name="layout"></param>
void VertexArray::AddBuffer(const std::shared_ptr<VertexBuffer>& vb, const VertexBufferLayout& layout)
{
	this->vbList.push_back(vb);
	this->attributeCountList.push_back(0);

	Bind();
	vb->Bind();

	if (layout.GetTotalCount() == 0)
		return; //layout有可能没有内容

	if (m_attribeIndex == 0) {
		m_vertexCount += vb->GetCount() / layout.GetTotalCount();
	}

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCALL(glEnableVertexAttribArray(m_attribeIndex));
		GLCALL(glVertexAttribPointer(m_attribeIndex, element.count, element.type,
			element.normalized, layout.GetStride(), (const void*)offset));
		offset += element.size();
		glVertexAttribDivisor(m_attribeIndex, element.instanceDivisor);
		m_attribeIndex++;
		attributeCountList.back()++;
	}

	vb->Unbind();
	UnBind();
}

void VertexArray::PopVertexBuffer() {
	vbList.pop_back();
	m_attribeIndex -= attributeCountList.back();
	attributeCountList.pop_back();
}

void VertexArray::Bind() const
{
	GLCALL(glBindVertexArray(m_RendererID));
}

void VertexArray::UnBind() const
{
	GLCALL(glBindVertexArray(0));
}
