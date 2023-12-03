#pragma once
#include "Texture.h"

class Renderbuffer
{
private:
	unsigned int rbo;
public:
	Renderbuffer();
	~Renderbuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetId() const { return rbo; }
};

