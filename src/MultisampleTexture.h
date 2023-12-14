#pragma once
class MultisampleTexture
{
public:
	MultisampleTexture(unsigned int multisample, unsigned int width, unsigned int height);
	~MultisampleTexture();
	void Bind() const;
	void Unbind() const;
	unsigned int GetId() { return m_id; }
private:
	unsigned int m_id = 0;
	unsigned int multisample;
	unsigned int width;
	unsigned int height;
};

