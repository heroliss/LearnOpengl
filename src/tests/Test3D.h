#pragma once

#include "test.h"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "../res/Materials/3DBaseMaterial.h" //载入材质的象征性做法
#include "../res/Materials/NormalDisplayMaterial.h" //载入材质的象征性做法
#include "../res/Materials/LightDisplayMaterial.h" //载入材质的象征性做法
#include "../res/Materials/SingleColorMaterial.h" //载入材质的象征性做法

#include "../res/Materials/PostProcess/ResizeViewportMaterial.h"
#include "../res/Materials/PostProcess/InverseColor.h"
#include "../res/Materials/PostProcess/Grayscale.h"
#include "../res/Materials/PostProcess/KernelEffect.h"

#include "Model.h"

namespace test {
	class Test3D : public Test
	{
	public:
		Test3D();
		~Test3D();

		void OnUpdate(float deltaTime) override;
		void AddToDrawList(const VertexArray& va, std::shared_ptr<Material> material, glm::mat4 modelMatrix, const IndexBuffer* ib = nullptr, unsigned int mode = GL_TRIANGLES, const unsigned int count = 0);
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> va_centerPoint;
		std::unique_ptr<VertexArray> va;
		std::unique_ptr<IndexBuffer> ib;
		std::shared_ptr<BaseMaterial3D> mainMaterial;
		std::unique_ptr<NormalDisplayMaterial> normalDisplayMaterial;
		std::unique_ptr<LightDisplayMaterial> lightDisplayMaterial;
		std::unique_ptr<SingleColorMaterial> singleColorMaterial;
		std::unique_ptr<SingleColorMaterial> zeroColorMaterial;
		bool showDepth;
		glm::vec2 showDepthRange = glm::vec2(1, 500); //预设的深度图范围
		bool showOutline;
		bool showLine;
		bool showPoint;
		bool canSeeThrough; //showLine和showPoint时是否可以看到后面
		bool mutiCubes;
		bool enableNormalTexture = true;
		bool enableMainTexture = true;
		bool enableSpecularTexture = true;
		int randomSeed;

		std::vector<std::shared_ptr<Light>>& lights = Application::GetInstance()->renderer->lights;

		int selectedItemIndex = -1;

		int currentModelIndex = 0;
		const char* modelNames[3] = { "cube" , "character", "car" };
		const char* modelPaths[3] = { "", "res/Models/nanosuit/nanosuit.obj", "res/Models/rs6/rs6.obj" };
		const float modelScales[3] = { 100, 30, 100 };
		const glm::vec3 modelOffsets[3] = { glm::vec3(0), glm::vec3(0, -250, 0), glm::vec3(0) };
		glm::mat4 GetCurrentModelMatrix() {
			//设置大小和偏移
			glm::mat4 modelMatrix = glm::translate(Application::GetInstance()->input->ModelMatrix, modelOffsets[currentModelIndex]);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(modelScales[currentModelIndex]));
			return modelMatrix;
		}

		std::vector<std::shared_ptr<PostProcessingMaterial>> postProcessMaterials = {
			std::make_shared<KernelEffect>(),
			std::make_shared<Grayscale>(),
			std::make_shared<InverseColor>(),
			std::make_shared<ResizeViewportMaterial>(),
		};
	};
}

