#include <iostream>
#include "renderer.h"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_clip_space.hpp"

Renderer::Renderer(int viewportWidth, int viewportHeight, int windowWidth, int windowHeight)
	: m_ClearColor{ 0,0,0,1 },
	ViewportWidth(viewportWidth),
	ViewportHeight(viewportHeight),
	WindowWidth(windowWidth),
	WindowHeight(windowHeight)
{

}

Renderer::~Renderer()
{
}

void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	// 在控制台输出调试消息
	std::cout << "OpenGL Debug Message: " << message << std::endl;
}

int Renderer::Init()
{
	// glad: 加载所有OpenGL函数指针
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //加载OpenGL函数指针
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//开启opengl的Debug信息输出
	//GLCALL(glEnable(GL_DEBUG_OUTPUT));
	//GLCALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
	//GLCALL(glDebugMessageCallback(MessageCallback, 0));

	std::cout << glGetString(GL_VERSION) << std::endl; //显示gl版本
	int maxVertexAttribs;
	GLCALL(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs));
	std::cout << "Max vertex attribs:" << maxVertexAttribs << std::endl; //显示最大属性数量

	ResetViewportSize(ViewportWidth, ViewportHeight);
	//ResetCamera();

	//设置混合模式
	GLCALL(glEnable(GL_BLEND));
	GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// 启用背面剔除
	GLCALL(glEnable(GL_CULL_FACE));
	GLCALL(glCullFace(GL_BACK));// 设置背面剔除模式
	GLCALL(glFrontFace(GL_CCW));// 设置剔除面:逆时针顺序连接的面为正面 (默认为逆时针)

	//启用深度测试
	SetDepthTestActive(true);
	GLCALL(glDepthFunc(GL_LEQUAL));  //默认是GL_LESS （绘制天空盒需要equal）
	//GLCALL(glDepthMask(false));

	//启用深度偏移
	ApplyPolygonOffsetFactor();

	//关闭模板测试
	GLCALL(glDisable(GL_STENCIL_TEST));

	//线型和线宽
	GLCALL(glLineWidth(1)); //width为float类型值，0到1
	GLCALL(glEnable(GL_LINE_SMOOTH)); //设置小数值才起作用，否则就四舍五入的处理整数了

	//抗锯齿
	GLCALL(glEnable(GL_MULTISAMPLE)); //(默认开启MSAA)

	//初始化后处理用的自定义帧缓冲 和 MASS用的多重采样帧缓冲
	InitFrameBufferRendering();

	return 0;
}

void Renderer::SetColorDiscard(bool discardColor) {
	if (discardColor) {
		GLCALL(glBlendFunc(GL_ZERO, GL_ONE));
	}
	else {
		GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	}
}

void Renderer::ResetViewportSize(int viewportWidth, int viewportHeight)
{
	ViewportWidth = viewportWidth;
	ViewportHeight = viewportHeight;
	ApplyViewportSize(viewportWidth, viewportHeight, true, true);
}

void Renderer::ApplyViewportSize(int viewportWidth, int viewportHeight, bool createNewFrameBuffer, bool resetCameraAspect)
{
	// 确保视口与新窗口尺寸匹配；注意，对于Retina显示器，width和height将比指定的大得多。
	GLCALL(glViewport(0, 0, viewportWidth, viewportHeight)); // 设置OpenGL渲染
	if (viewportWidth != 0 && viewportHeight != 0)  //最小化窗口时宽高会变成0！
	{
		if (resetCameraAspect)
		{
			camera.aspect = GetViewportAspect();
			camera.UpdateOrthoRectByViewport(viewportWidth, viewportHeight);
			camera.UpdateProjectionMatrix();
		}
		if (createNewFrameBuffer)
		{
			InitFrameBufferRendering(); //窗口尺寸变化后需要重新创建FrameBuffer
		}
	}
}

const float* Renderer::GetClearColor() const
{
	return m_ClearColor;
}

void Renderer::Clear() const
{
	stepCount++;
	if (stepCount > stepCountLimit) return; //限制渲染步数，用于图像调试
	clearCount++;

	GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void Renderer::ClearColorBuffer() const
{
	stepCount++;
	if (stepCount > stepCountLimit) return; //限制渲染步数，用于图像调试
	clearCount++;

	GLCALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::SetClearColor(float* color, int length)
{
	for (int i = 0; i < length; i++)
	{
		m_ClearColor[i] = color[i];
	}
	GLCALL(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
}

void Renderer::SetClearColor(float r, float g, float b, float a)
{
	m_ClearColor[0] = r;
	m_ClearColor[1] = g;
	m_ClearColor[2] = b;
	m_ClearColor[3] = a;
	GLCALL(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
}

//渲染顶点数据，ib可以为空
bool Renderer::Draw(const VertexArray& va, const Material& material, glm::mat4 modelMatrix, const IndexBuffer* ib, const unsigned int instanceCount, unsigned int mode, const unsigned int count) const
{
	stepCount++;
	if (stepCount > stepCountLimit) return false; //限制渲染步数，用于图像调试
	drawCallCount++;

	auto shader = material.GetShader();
	shader->Bind();
	material.ApplyUniforms(modelMatrix);
	va.Bind();
	int verticesCount = count;
	if (ib != nullptr)
	{
		if (count == 0) verticesCount = ib->GetCount();
		ib->Bind();
		GLCALL(glDrawElementsInstanced(mode, verticesCount, GL_UNSIGNED_INT, nullptr, instanceCount));
		ib->Unbind();
	}
	else {
		if (count == 0) verticesCount = va.GetVertexCount();
		GLCALL(glDrawArraysInstanced(mode, 0, verticesCount, instanceCount));
	}
	va.UnBind();
	primitiveCountInfo += CalculateRenderCounts(verticesCount * instanceCount, mode);
	return true;
}

PrimitiveCountInfo Renderer::CalculateRenderCounts(int numVertices, unsigned int renderMode) const {
	PrimitiveCountInfo info;
	info.numVertices = numVertices;
	switch (renderMode) {
	case GL_POINTS:
		info.numPoints = numVertices;
		break;
	case GL_LINES:
		info.numLineSegments = numVertices / 2;
		break;
	case GL_LINE_LOOP:
		info.numLineSegments = numVertices;
		break;
	case GL_TRIANGLES:
		info.numTriangles = numVertices / 3;
		break;
	case GL_TRIANGLE_STRIP:
		info.numTriangles = numVertices - 2;
		break;
	case GL_TRIANGLE_FAN:
		info.numTriangles = numVertices - 2;
		break;
	case GL_QUADS:
		info.numQuads = numVertices / 4;
		break;
	case GL_QUAD_STRIP:
		info.numQuads = (numVertices - 2) / 2;
		break;
	default:
		std::cout << "Exception: Invalid render mode!" << std::endl;
	}
	return info;
}