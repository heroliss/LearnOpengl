#pragma once

#define VertexBufferParams_array(x) x, sizeof(x), sizeof(x) / sizeof(x[0])
#define VertexBufferParams_vector(x) x.data(), x.size() * sizeof(x[0]), x.size()


class VertexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	VertexBuffer(const void* data, unsigned int size, unsigned int count, bool dynamicDraw = false);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void SetData(const void* data, const long size, const long offset) const;

	inline unsigned int GetCount() const { return m_Count; }
};