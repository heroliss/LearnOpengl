#pragma once
#include "Texture.h"
#include "Renderbuffer.h"
#include "MultisampleTexture.h"

class Framebuffer
{
private:
	unsigned int m_RendererID;
	unsigned int multisample;
	std::shared_ptr<Texture> m_texture;
	Renderbuffer m_renderBuffer;
	//Renderbuffer m_colorRenderBuffer;
	std::shared_ptr<MultisampleTexture> m_multisampleTexture;

public:
	Framebuffer(unsigned int multisample = 0);
	~Framebuffer();

	void Bind() const;
	void BindRead() const;
	void BindDraw() const;
	static void BindDefault() {
		GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
	static void BindDefaultRead() {
		GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
	}
	static void BindDefaultDraw() {
		GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	}

	inline unsigned int GetId() const { return m_RendererID; }
	inline unsigned int GetMultiSample() const { return multisample; }
	inline std::shared_ptr<Texture> GetTexture() { return m_texture; }
};

