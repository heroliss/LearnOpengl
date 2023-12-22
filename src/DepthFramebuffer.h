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
	int width; //这里没用unsigned只是因为方便绑定到imgui
	int height;

	DepthFramebuffer(unsigned int width, unsigned int height);
	~DepthFramebuffer();
	void SetDepthTextureAndBind(std::shared_ptr<Texture> texture);

	void Bind() const;
	void BindRead() const;
	void BindDraw() const;

	inline unsigned int GetId() const { return m_RendererID; }
	inline std::shared_ptr<Texture> GetDepthTexture() const { return m_texture; }
};

