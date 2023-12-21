#pragma once

class UniformBuffer
{
private:
	unsigned int m_RendererID;
public:
	UniformBuffer(const void* data, unsigned long long int size, bool dynamicDraw = false);
	~UniformBuffer();

	void Bind() const;
	void Unbind() const;

	void SetData(const void* data, const long size, const long offset) const;

	void BindPoint(unsigned int bindingPoint, unsigned long long int size = 0, unsigned long long int startOffset = 0);
};