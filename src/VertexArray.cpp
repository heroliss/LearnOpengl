#include "VertexArray.h"

VertexArray::VertexArray()
{
	GLCALL(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCALL(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const std::shared_ptr<VertexBuffer>& vb, const VertexBufferLayout& layout)
{
	this->vb = vb;
	vb->Bind();
	Bind();
	if (layout.GetTotalCount() == 0) return; //layout有可能没有内容
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCALL(glEnableVertexAttribArray(i));
		GLCALL(glVertexAttribPointer(i, element.count, element.type,
			element.normalized, layout.GetStride(), (const void*)offset));
		offset += element.size();
	}
	m_vertexCount = vb->GetCount() / layout.GetTotalCount();
}

void VertexArray::Bind() const
{
	GLCALL(glBindVertexArray(m_RendererID));
}

void VertexArray::UnBind() const
{
	GLCALL(glBindVertexArray(0));
}
