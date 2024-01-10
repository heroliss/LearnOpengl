#pragma once
#include "Texture.h"
#include "Renderbuffer.h"
#include "MultisampleTexture.h"

class Framebuffer
{
private:
	unsigned int m_RendererID;
	unsigned int multisample;
	std::vector<std::shared_ptr<Texture>> m_textures;
	Renderbuffer m_renderBuffer;
	//Renderbuffer m_colorRenderBuffer;
	std::vector<std::shared_ptr<MultisampleTexture>> m_multisampleTextures;

public:
	Framebuffer(unsigned int mrtNum, unsigned int multisample);
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
	inline const std::vector<std::shared_ptr<Texture>>& GetTextures() const { return m_textures; }
};

