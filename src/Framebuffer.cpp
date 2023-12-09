#include "GLCALL.h"
#include "Framebuffer.h"
#include "Application.h"

Framebuffer::Framebuffer()
{
	GLCALL(glGenFramebuffers(1, &m_RendererID));

	auto app = Application::GetInstance();
	int width = app->renderer->ViewportWidth;
	int height = app->renderer->ViewportHeight;

	Bind();

	//设置颜色附件
	m_texture = std::make_shared<Texture>();
	m_texture->bind();
	m_texture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_texture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	m_texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
	GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetId(), 0)); //附加颜色附件到帧缓冲
	m_texture->unbind();

	//设置颜色附件
	//m_colorRenderBuffer.Bind();
	//GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height));
	//GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderBuffer.GetId());); //附加深度和模板附件到帧缓冲
	//m_colorRenderBuffer.Unbind();

	//设置深度和模板附件
	m_renderBuffer.Bind();
	GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer.GetId());); //附加深度和模板附件到帧缓冲
	m_renderBuffer.Unbind();

	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	Unbind();
}

Framebuffer::~Framebuffer()
{
	//std::cout << "Delete: FrameBuffer " << m_RendererID << std::endl;
	GLCALL(glDeleteFramebuffers(1, &m_RendererID));
}

void Framebuffer::Bind() const
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void Framebuffer::Unbind() const
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
