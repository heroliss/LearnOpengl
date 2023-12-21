#pragma once

#include <memory>
#include "GLCALL.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"
#include "Framebuffer.h"
#include "PostProcessingMaterial.h"
#include "UniformBuffer.h"
#include <map>
#include "glm/gtc/type_ptr.hpp"

struct PrimitiveCountInfo
{
	int numVertices = 0;
	int numPoints = 0;
	int numLineSegments = 0;
	int numTriangles = 0;
	int numQuads = 0;
	PrimitiveCountInfo operator+(const PrimitiveCountInfo& other) const {
		PrimitiveCountInfo result;
		result.numVertices = this->numVertices + other.numVertices;
		result.numPoints = this->numPoints + other.numPoints;
		result.numLineSegments = this->numLineSegments + other.numLineSegments;
		result.numTriangles = this->numTriangles + other.numTriangles;
		result.numQuads = this->numQuads + other.numQuads;
		return result;
	}
	PrimitiveCountInfo operator-(const PrimitiveCountInfo& other) const {
		PrimitiveCountInfo result;
		result.numVertices = this->numVertices - other.numVertices;
		result.numPoints = this->numPoints - other.numPoints;
		result.numLineSegments = this->numLineSegments - other.numLineSegments;
		result.numTriangles = this->numTriangles - other.numTriangles;
		result.numQuads = this->numQuads - other.numQuads;
		return result;
	}
	PrimitiveCountInfo& operator+=(const PrimitiveCountInfo& other) {
		this->numVertices += other.numVertices;
		this->numPoints += other.numPoints;
		this->numLineSegments += other.numLineSegments;
		this->numTriangles += other.numTriangles;
		this->numQuads += other.numQuads;
		return *this;
	}
	PrimitiveCountInfo& operator-=(const PrimitiveCountInfo& other) {
		this->numVertices -= other.numVertices;
		this->numPoints -= other.numPoints;
		this->numLineSegments -= other.numLineSegments;
		this->numTriangles -= other.numTriangles;
		this->numQuads -= other.numQuads;
		return *this;
	}
};

struct DrawInfo {
	DrawInfo(const VertexArray* va, std::shared_ptr<Material> material, glm::mat4 modelMatrix, const IndexBuffer* ib = nullptr, unsigned int mode = GL_TRIANGLES, const unsigned int count = 0, const unsigned int instanceCount = 1) :
		va(va), material(material), modelMatrix(modelMatrix), ib(ib), mode(mode), count(count), depth(0), instanceCount(instanceCount)
	{
	}
	const VertexArray* va;
	std::shared_ptr<Material> material;
	glm::mat4 modelMatrix;
	const IndexBuffer* ib;
	unsigned int mode;
	unsigned int count;
	unsigned int instanceCount;

	float depth; //距离摄像机的距离，用于排序

	DrawInfo& operator=(const DrawInfo& other) {
		va = other.va;
		material = other.material;
		modelMatrix = other.modelMatrix;
		ib = other.ib;
		mode = other.mode;
		count = other.count;
		instanceCount = other.instanceCount;
		depth = other.depth;
		return *this;
	}
};

class Renderer
{
public:
	Renderer(int viewportWidth, int viewportHeight, int windowWidth, int windowHeight);
	~Renderer();

	int WindowWidth;
	int WindowHeight;
	int ViewportWidth;
	int ViewportHeight;
	float GetViewportAspect() { return (float)ViewportWidth / ViewportHeight; }
	unsigned int multiSample = 0; //用于自定义的多重采样framebuffer

	Camera camera;
	mutable PrimitiveCountInfo primitiveCountInfo;
	mutable int stepCount = 0;
	mutable int drawCallCount = 0;
	mutable int clearCount = 0;
	int postProcessCount = 0;
	int stepCountLimit = 1000000000;

	//-------------------UniformBuffer----------------------
	std::shared_ptr<UniformBuffer> matricesUniformBuffer;
	std::shared_ptr<UniformBuffer> lightsUniformBuffer;
	void InitUniformBuffers() {
		//视图矩阵和投影矩阵
		matricesUniformBuffer = std::make_shared<UniformBuffer>(nullptr, sizeof(glm::mat4) * 2);
		matricesUniformBuffer->BindPoint(0/*, sizeof(glm::mat4) * 2, 0  这里后两个参数可以省略*/);
		lightsUniformBuffer = std::make_shared<UniformBuffer>(nullptr, 16 + Light::GetSize() * MAX_LIGHT_COUNT);
		lightsUniformBuffer->BindPoint(1);
	}

	//--------------------FrameBuffer-----------------------
	bool enablePostProcessing = true;
	std::vector<std::shared_ptr<PostProcessingMaterial>> postProcessingMaterials;
	std::shared_ptr<Framebuffer> currentFrameBuffer; //空代表默认帧缓冲
	std::shared_ptr<Framebuffer> frameBuffer1;
	std::shared_ptr<Framebuffer> frameBuffer2;
	std::shared_ptr<Framebuffer> multiSampleFrameBuffer;
	float frameBuffer_quadVertices[4 * 6] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	std::shared_ptr<VertexArray> frameBuffer_va;
	void InitFrameBufferRendering()
	{
		if (frameBuffer_va == nullptr)
		{
			frameBuffer_va = std::make_shared<VertexArray>();
			auto vb = std::make_shared<VertexBuffer>(VertexBufferParams_array(frameBuffer_quadVertices));
			VertexBufferLayout layout;
			layout.Push<float>(2);
			layout.Push<float>(2);
			frameBuffer_va->AddBuffer(vb, layout);
		}
		frameBuffer1 = std::make_shared<Framebuffer>();
		frameBuffer2 = std::make_shared<Framebuffer>();
		SetMultiSample(multiSample);
	}

	void SetMultiSample(unsigned int multiSample)
	{
		this->multiSample = multiSample;
		multiSampleFrameBuffer = multiSample == 0 ? nullptr : std::make_shared<Framebuffer>(multiSample);
	}

	void BindCurrentFrameBuffer() {
		if (currentFrameBuffer == nullptr)
			Framebuffer::BindDefault();
		else
			currentFrameBuffer->Bind();
		Clear();
	}
	void BindMultiSampleFrameBuffer()
	{
		multiSampleFrameBuffer->Bind();
		Clear();
	}
	void SwitchAnotherCustomFrameBuffer() {
		if (currentFrameBuffer == frameBuffer1) currentFrameBuffer = frameBuffer2;
		else currentFrameBuffer = frameBuffer1;
		BindCurrentFrameBuffer();
	}
	void SwitchDefaultFrameBuffer() {
		currentFrameBuffer = nullptr;
		BindCurrentFrameBuffer();
	}
	void DrawAllPostProcessing() {
		int materialsCount = postProcessingMaterials.size();
		for (int i = 0; i < materialsCount; i++)
		{
			//后处理渲染包括两步：clear和draw，但合并成一步
			if (stepCount >= stepCountLimit)
			{
				stepCount++;
				continue;
			}

			//获取当前帧缓冲的图像
			auto material = postProcessingMaterials[i];
			material->ScreenTexture = currentFrameBuffer->GetTexture();

			stepCount--; //为合并一步抵消下面clear中的stepCount++
			//若是最后一个后处理，或是最后两个渲染步骤，则渲染到屏幕
			if (i == materialsCount - 1 || stepCount + 2 >= stepCountLimit)
			{
				SwitchDefaultFrameBuffer();
			}
			else //否则渲染到自定义帧缓冲
			{
				SwitchAnotherCustomFrameBuffer();
			}

			Draw(*frameBuffer_va, *material);
			postProcessCount++;
		}
	}
	bool HasPostProcessingToDraw() {
		int postProcessingMaterialsCount = postProcessingMaterials.size();
		return enablePostProcessing && postProcessingMaterialsCount > 0 && stepCountLimit > stepCount - postProcessingMaterialsCount;
	}
	void BlitMultiSampleFramebufferToCurrentFrameBuffer() {
		multiSampleFrameBuffer->BindRead();
		if (currentFrameBuffer == nullptr) Framebuffer::BindDefaultDraw();
		else currentFrameBuffer->BindDraw();
		GLCALL(glBlitFramebuffer(0, 0, ViewportWidth, ViewportHeight, 0, 0, ViewportWidth, ViewportHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST));
	}
	//----------------------------------------------------

	/// <summary>
	/// 恢复统计数据，包括渲染步数、drawcall次数、图元数量统计
	/// </summary>
	void ResetStatisticData() {
		stepCount = 0;
		drawCallCount = 0;
		clearCount = 0;
		postProcessCount = 0;
		primitiveCountInfo = PrimitiveCountInfo();
	}

	glm::vec2 polygonOffsetFactor = glm::vec2(1, 1);
	void ApplyPolygonOffsetFactor() {
		GLCALL(glEnable(GL_POLYGON_OFFSET_FILL));
		GLCALL(glPolygonOffset(polygonOffsetFactor.x, polygonOffsetFactor.y));
	}

	//光照
	std::vector<std::shared_ptr<Light>> lights;

	std::shared_ptr<Light> AddLight() {
		auto light = std::make_shared<Light>();
		lights.push_back(light);
		int lightsNum = lights.size();
		lightsUniformBuffer->SetData(&lightsNum, 4, 0);
		lightsUniformBuffer->SetData(light->GetData(), Light::GetSize(), 16 + (lightsNum - 1) * Light::GetSize());

		//std::cout << offsetof(Light, type) << std::endl;
		//std::cout << offsetof(Light, useBlinnPhong) << std::endl;
		//std::cout << offsetof(Light, pos) << std::endl;
		//std::cout << offsetof(Light, direction) << std::endl;
		//std::cout << offsetof(Light, color) << std::endl;
		//std::cout << offsetof(Light, attenuation) << std::endl;
		//std::cout << offsetof(Light, cutoffAngle) << std::endl;

		return light;
	}

	void RemoveLight(int index) {
		lights.erase(lights.begin() + index);
	}

	void ClearLights() {
		lights.clear();
	}

	void ApplyLights() {
		int lightsNum = lights.size();
		lightsUniformBuffer->SetData(&lightsNum, 4, 0);
		for (int i = 0; i < lightsNum; i++)
		{
			auto light = lights[i];
			lightsUniformBuffer->SetData(light->GetData(), Light::GetSize(), 16 + i * Light::GetSize());
		}
	}

	void ResetCamera() {
		camera = Camera();
		camera.ViewMatrixChangedEvent.AddHandler([this](glm::mat4 ViewMatrix) {
			matricesUniformBuffer->SetData(glm::value_ptr(ViewMatrix), sizeof(glm::mat4), 0); });
		camera.ProjectionMatrixChangedEvent.AddHandler([this](glm::mat4 ProjectionMatrix) {
			matricesUniformBuffer->SetData(glm::value_ptr(ProjectionMatrix), sizeof(glm::mat4), sizeof(glm::mat4)); });
		camera.aspect = GetViewportAspect();
		camera.UpdateOrthoRectByViewport(ViewportWidth, ViewportHeight);
		camera.UpdateProjectionMatrix();
		camera.UpdateViewMatrix();
	}

	int Init();
	void ResetViewportSize(int viewportWidth, int viewportHeight);
	void ApplyViewportSize(int viewportWidth, int viewportHeight, bool createNewFrameBuffer, bool resetCameraAspect);
	bool NeedReapplyCurrentViewportSize = false;

	void Clear() const;
	void ClearColorBuffer() const;
	void SetClearColor(float* color, int length);
	void SetClearColor(float r, float g, float b, float a);
	const float* GetClearColor() const;

	std::vector<DrawInfo> opaqueDrawList;
	std::vector<DrawInfo> transparentDrawList;
	std::vector<DrawInfo> combineDrawList;
	//std::map<float, DrawInfo> transparentDrawMap;

	bool Draw(const VertexArray& va, const Material& material, glm::mat4 modelMatrix = glm::mat4(1), const IndexBuffer* ib = nullptr, const unsigned int instanceCount = 1, unsigned int mode = GL_TRIANGLES, const unsigned int count = 0) const;

	void SetPolygonFillMode() {
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
	}
	void SetPolygonLineMode() {
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
	void SetPolygonPointMode() {
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, GL_POINT));
	}
	void SetColorDiscard(bool discardColor);

	PrimitiveCountInfo CalculateRenderCounts(int numVertices, unsigned int renderMode) const;
	void SetDepthTestActive(bool active) { if (active) GLCALL(glEnable(GL_DEPTH_TEST)) else GLCALL(glDisable(GL_DEPTH_TEST)); }
private:
	float m_ClearColor[4];
};