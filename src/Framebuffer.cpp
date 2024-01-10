#include "GLCALL.h"
#include "Framebuffer.h"
#include "Application.h"

Framebuffer::Framebuffer(unsigned int mrtNum, unsigned int multisample) : multisample(multisample)
{
	GLCALL(glGenFramebuffers(1, &m_RendererID));

	auto app = Application::GetInstance();
	int width = app->renderer->ViewportWidth;
	int height = app->renderer->ViewportHeight;

	Bind();

	//设置多个颜色附件 MRT（Multiple Render Targets，多渲染目标）
	for (int i = 0; i < mrtNum; i++)
	{
		if (multisample == 0) {
			std::shared_ptr<Texture> texture = std::make_shared<Texture>();
			m_textures.push_back(texture);
			texture->bind();
			texture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			texture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
			GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL));  //这里使用支持HDR的格式
			GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->GetId(), 0)); //附加颜色附件到帧缓冲
			texture->unbind();
		}
		else
		{
			std::shared_ptr<MultisampleTexture> multisampleTexture = std::make_shared<MultisampleTexture>(multisample, width, height);
			m_multisampleTextures.push_back(multisampleTexture);

			//设置颜色附件(使用mass的采样数设定多重采样纹理)
			multisampleTexture->Bind();
			GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, multisampleTexture->GetId(), 0)); //附加颜色附件到帧缓冲
			multisampleTexture->Unbind();	
			
			//设置颜色附件(RenderBuffer，不能进行纹理过滤和采样)
			//m_colorRenderBuffer.Bind();
			//GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height));
			//GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_colorRenderBuffer.GetId());); //附加深度和模板附件到帧缓冲
			//m_colorRenderBuffer.Unbind();
		}
	}

	//设置深度和模板附件
	m_renderBuffer.Bind();
	if (multisample > 0) {
		GLCALL(glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, GL_DEPTH24_STENCIL8, width, height));
	}
	else
	{
		GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	}
	GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer.GetId());); //附加深度和模板附件到帧缓冲
	m_renderBuffer.Unbind();

	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	//Unbind(); //没有解绑操作，实例化FrameBuffer后将保持在当前帧缓冲
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

void Framebuffer::BindRead() const
{
	GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID));
}

void Framebuffer::BindDraw() const
{
	GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID));
}