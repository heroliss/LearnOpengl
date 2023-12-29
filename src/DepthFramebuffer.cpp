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

void DepthFramebuffer::SetDepthTextureAndBind(std::shared_ptr<Texture> texture)
{
	m_texture = texture;
	Bind();
	//用2D纹理设置深度附件
	m_texture->bind();
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; //让所有超出深度贴图坐标范围的深度值为1.0
	m_texture->setParameter(GL_TEXTURE_BORDER_COLOR, borderColor);
	m_texture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	m_texture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	m_texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST); //这里用GL_NEAREST，因为对深度图做线性模糊毫无意义
	m_texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture->GetId(), 0)); //附加颜色附件到帧缓冲
	m_texture->unbind();

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