#include "GLCALL.h"
#include "DepthFramebuffer.h"
#include "Application.h"

DepthFramebuffer::DepthFramebuffer()
{
	GLCALL(glGenFramebuffers(1, &m_RendererID));

	auto app = Application::GetInstance();

	Bind();

	//不需要颜色附件
	GLCALL(glDrawBuffer(GL_NONE));
	GLCALL(glReadBuffer(GL_NONE));

	//Unbind(); //没有解绑操作，实例化FrameBuffer后将保持在当前帧缓冲
}

DepthFramebuffer::~DepthFramebuffer()
{
	//std::cout << "Delete: FrameBuffer " << m_RendererID << std::endl;
	GLCALL(glDeleteFramebuffers(1, &m_RendererID));
}

void DepthFramebuffer::SetDepthCubemapAndBind(std::shared_ptr<Cubemap> cubemap)
{
	m_cubemap = cubemap;
	Bind();
	GLCALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_cubemap->GetId(), 0));	
	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}

void DepthFramebuffer::SetDepthTextureAndBind(std::shared_ptr<Texture> texture)
{
	m_texture = texture;
	Bind();
	GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture->GetId(), 0));
	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}


void DepthFramebuffer::Bind() const
{
	GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void DepthFramebuffer::BindRead() const
{
	GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID));
}

void DepthFramebuffer::BindDraw() const
{
	GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID));
}