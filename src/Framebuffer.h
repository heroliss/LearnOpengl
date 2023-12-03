#pragma once
#include "Texture.h"
#include "Renderbuffer.h"

class Framebuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
	std::shared_ptr<Texture> m_texture;
	Renderbuffer m_renderBuffer;
	Renderbuffer m_colorRenderBuffer;
public:
	Framebuffer();
	~Framebuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
	inline unsigned int GetId() const { return m_RendererID; }
	inline std::shared_ptr<Texture> GetTexture() { return m_texture; }
};

