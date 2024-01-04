#pragma once
#include "Texture.h"
#include "Renderbuffer.h"
#include "Texture.h"
#include "Cubemap.h"

class DepthFramebuffer
{
private:
	unsigned int m_RendererID;
	std::shared_ptr<Texture> m_texture;
	std::shared_ptr<Cubemap> m_cubemap;
	Renderbuffer m_renderBuffer;

public:
	DepthFramebuffer();
	~DepthFramebuffer();
	void SetDepthCubemapAndBind(std::shared_ptr<Cubemap> cubemap);
	void SetDepthTextureAndBind(std::shared_ptr<Texture> texture);

	void Bind() const;
	void BindRead() const;
	void BindDraw() const;

	inline unsigned int GetId() const { return m_RendererID; }
	inline std::shared_ptr<Texture> GetDepthTexture() const { return m_texture; }
};

