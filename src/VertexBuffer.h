#pragma once

#define VertexBufferParams_array(x) x, sizeof(x), sizeof(x) / sizeof(x[0])
#define VertexBufferParams_vector(x) x.data(), x.size() * sizeof(x[0]), x.size()


class VertexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	VertexBuffer(const void* data, unsigned int size, unsigned int count);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};