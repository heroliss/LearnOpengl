#pragma once

#include <vector>
#include "GLCALL.h"

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
	unsigned int size() const {
		return GetSizeOfType(type) * count;
	}

	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type) {
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
	unsigned int m_totalCount;
public:
	VertexBufferLayout() : m_Stride(0), m_totalCount(0) {}

	template<typename T>
	void Push(unsigned int count) {
		//static_assert(false);
		ASSERT(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		VertexBufferElement e = { GL_FLOAT, count, GL_FALSE };
		m_Elements.push_back(e);
		m_Stride += e.size();
		m_totalCount += count;
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		VertexBufferElement e = { GL_UNSIGNED_INT, count, GL_FALSE };
		m_Elements.push_back(e);
		m_Stride += e.size();
		m_totalCount += count;
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		VertexBufferElement e = { GL_UNSIGNED_BYTE, count, GL_TRUE };
		m_Elements.push_back(e);
		m_Stride += e.size();
		m_totalCount += count;
	}

	inline const std::vector<VertexBufferElement> GetElements() const {
		return m_Elements;
	}
	inline unsigned int GetStride() const { return m_Stride; }
	inline unsigned int GetTotalCount() const { return m_totalCount; }
};

