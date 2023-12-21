#pragma once
#include "Texture.h"
#include "Renderbuffer.h"
#include "MultisampleTexture.h"

class DepthFramebuffer
{
private:
	unsigned int m_RendererID;
	std::shared_ptr<Texture> m_texture;
	Renderbuffer m_renderBuffer;

public:
	DepthFramebuffer();
	~DepthFramebuffer();

	void Bind() const;
	void BindRead() const;
	void BindDraw() const;

	inline unsigned int GetId() const { return m_RendererID; }
	inline std::shared_ptr<Texture> GetDepthTexture() { return m_texture; }
};

