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
#include "../res/Materials/TexcoordDisplayMaterial.h" //载入材质的象征性做法

#include "../res/Materials/PostProcess/ResizeViewportMaterial.h"
#include "../res/Materials/PostProcess/InverseColor.h"
#include "../res/Materials/PostProcess/Grayscale.h"
#include "../res/Materials/PostProcess/KernelEffect.h"
#include "../res/Materials/PostProcess/GammaCorrection.h"

#include "Model.h"
#include "Skybox.h"

namespace test {
	class Test3D : public Test
	{
	public:
		Test3D();
		~Test3D();

		void OnUpdate(float deltaTime) override;
		void AddToDrawList(const VertexArray& va, std::shared_ptr<Material> material, glm::mat4 modelMatrix, const IndexBuffer* ib = nullptr, const unsigned int instanceCount = 1, unsigned int mode = GL_TRIANGLES, const unsigned int count = 0);
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Skybox skybox;
		std::unique_ptr<VertexArray> va_centerPoint;
		std::unique_ptr<VertexArray> va;
		std::unique_ptr<IndexBuffer> ib;
		std::shared_ptr<BaseMaterial3D> mainMaterial;
		std::unique_ptr<NormalDisplayMaterial> normalDisplayMaterial;
		std::unique_ptr<LightDisplayMaterial> lightDisplayMaterial;
		std::unique_ptr<SingleColorMaterial> singleColorMaterial;
		std::unique_ptr<SingleColorMaterial> zeroColorMaterial;
		std::unique_ptr<TexcoordDisplayMaterial> texcoordDisplayMaterial;
		bool showDepth;
		glm::vec2 showDepthRange = glm::vec2(1, 500); //预设的深度图范围
		bool showSkybox;
		bool showOutline;
		bool showLine;
		bool showPoint;
		bool showTexcoord;
		bool canSeeThrough; //showLine和showPoint时是否可以看到后面

		bool enableNormalTexture = true;
		bool enableMainTexture = true;
		bool enableSpecularTexture = true;
		int randomSeed;

		std::vector<std::shared_ptr<Light>>& lights = Application::GetInstance()->renderer->lights;

		int selectedItemIndex = -1;

		int currentModelIndex = 0;
		const char* modelNames[7] = { "cube" , "sphere", "grid sphere", "character", "car", "planet", "rock" };
		const char* modelPaths[7] = { "", "res/Models/sphere.fbx", "res/Models/sphere_grid.3ds", "res/Models/nanosuit/nanosuit.obj", "res/Models/rs6/rs6.obj", "res/Models/planet/planet.obj", "res/Models/rock/rock.obj" };
		const float modelScales[7] = { 100, 100, 1, 30, 100, 30, 100 };
		const glm::vec3 modelOffsets[7] = { glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0, -250, 0), glm::vec3(0), glm::vec3(0), glm::vec3(0) };
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
			std::make_shared<GammaCorrection>(),
		};

		//比较笨的办法来让面板上的材质设置对所有物体生效
		void SetMaterialSameAsMainMaterial(std::shared_ptr<BaseMaterial3D> material) {
			//调试用：跟随主材质设置
			material->IsTransparent = mainMaterial->IsTransparent;
			material->ObjectColor = mainMaterial->ObjectColor;
			material->Ambient = mainMaterial->Ambient;
			material->Emission_inside = mainMaterial->Emission_inside;
			material->Emission_outside = mainMaterial->Emission_outside;
			material->SpecularColor = mainMaterial->SpecularColor;
			material->Shininess = mainMaterial->Shininess;
			material->enableRefract = mainMaterial->enableRefract;
			material->refractColor = mainMaterial->refractColor;
			material->refractiveIndex = mainMaterial->refractiveIndex;
		}

		//通用材质设置
		void CommonMaterialSetting(std::shared_ptr<BaseMaterial3D> material) {
			//设置环境立方体贴图
			material->cubemap = showSkybox ? skybox.GetCubemap() : nullptr;
			//设置是否显示深度
			material->showDepth = showDepth;
			material->showDepthRange = showDepthRange;
		}

		std::shared_ptr<BaseMaterial3D> CreateModelMaterial(const Model* model, const Mesh* mesh) {
			auto material = std::make_shared<BaseMaterial3D>(); //每帧新建材质(可优化)，每个mesh可能有不同的贴图设置
			//从网格信息中设置贴图和颜色
			material->SetFromAiMaterial(mesh->mat, model->directory, enableNormalTexture, enableMainTexture, enableSpecularTexture);
			SetMaterialSameAsMainMaterial(material);
			CommonMaterialSetting(material);
			return material;
		}

		//模型相关
		bool mutiCubes;

		std::shared_ptr<Model> rockModel;
		std::shared_ptr<glm::mat4[]> rockModelMatrices;
		std::shared_ptr<glm::vec3[]> asteroidRevolvesAxis;
		float planetMass = 10; //单位是兆吨
		float G = 6.67e-11;
		float planetTimeScale; //秒
		unsigned int asteroidBeltAmount;

		float asteroidBeltRadius = 200.0f;    //小行星带最内圈半径
		float asteroidBeltWidth = 400.0f;     //小行星带宽度（既xz方向向外拓展的距离）
		float asteroidBeltThickness = 100.0f; //小行星带厚度（既y方向的厚度）

		/// <summary>
		/// 初始化小行星带
		/// </summary>
		void InitAsteroid() {
			rockModel = Model::Get(modelPaths[6]);
			rockModelMatrices = std::make_shared<glm::mat4[]>(asteroidBeltAmount);
			asteroidRevolvesAxis = std::make_shared<glm::vec3[]>(asteroidBeltAmount);

			auto vb = std::make_shared<VertexBuffer>(nullptr, sizeof(glm::mat4) * asteroidBeltAmount, asteroidBeltAmount, true);
			VertexBufferLayout layout;
			layout.Push<float>(4, 1);
			layout.Push<float>(4, 1);
			layout.Push<float>(4, 1);
			layout.Push<float>(4, 1);
			for (int j = 0; j < rockModel->meshes.size(); j++)
			{
				auto mesh = &rockModel->meshes[j];
				mesh->va->AddBuffer(vb, layout);
			}

			InitAsteroidPos();
		}

		/// <summary>
		/// 小行星公转和自转
		/// </summary>
		void AsteroidRevolves() {
			if (planetTimeScale <= 0) return;
			//公转角速度公式：ω = √(G * M / r ^ 3)
			srand(0); //固定随机数序列
			float deltaTime = Application::GetInstance()->DeltaTime;
			for (int i = 0; i < asteroidBeltAmount; i++)
			{
				//公转
				if (planetMass > 0)
				{
					auto matrix = rockModelMatrices[i];
					float x = matrix[3].x;
					float y = matrix[3].y;
					float z = matrix[3].z;
					float r_3 = powf(x * x + y * y + z * z, 1.5f);
					float w = sqrtf(G * planetMass * 1000 * 1000 * 1000 / r_3);
					w *= planetTimeScale * deltaTime; //时间缩放
					matrix = glm::rotate(glm::mat4(1), w, asteroidRevolvesAxis[i]);
					rockModelMatrices[i] = matrix * rockModelMatrices[i];
				}
				//自转
				rockModelMatrices[i] = glm::rotate(rockModelMatrices[i], (float)rand() / RAND_MAX * planetTimeScale * deltaTime * 0.001f,
					glm::vec3((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX));
			}
			//修改VertexBuffer数据
			auto vb = rockModel->meshes[0].va->vbList[1]; //所有mesh的实例化属性共用同一个vb
			vb->SetData(&rockModelMatrices[0], sizeof(glm::mat4) * asteroidBeltAmount, 0);
		}

		/// <summary>
		/// 随机设置小行星带初始位置
		/// </summary>
		void InitAsteroidPos() {
			srand(randomSeed);
			for (unsigned int i = 0; i < asteroidBeltAmount; i++)
			{
				glm::mat4 model = glm::mat4(1);
				// 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
				float angle = (float)i / (float)asteroidBeltAmount * 360.0f;
				float displacement;

				displacement = asteroidBeltWidth == 0 ? 0 : ((float)rand() / RAND_MAX - 0.5) * asteroidBeltWidth;
				float x = sin(angle) * (asteroidBeltRadius + asteroidBeltWidth / 2.0f) + displacement;

				displacement = asteroidBeltWidth == 0 ? 0 : ((float)rand() / RAND_MAX - 0.5) * asteroidBeltWidth;
				float z = cos(angle) * (asteroidBeltRadius + asteroidBeltWidth / 2.0f) + displacement;

				displacement = asteroidBeltThickness == 0 ? 0 : ((float)rand() / RAND_MAX - 0.5) * asteroidBeltThickness;
				float y = displacement;

				model = glm::translate(model, glm::vec3(x, y, z));

				// 2. 缩放：在 1 和 5 之间缩放
				float scale = (rand() % 400) / 100.0f + 1.0f;
				model = glm::scale(model, glm::vec3(scale));

				// 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
				float rotAngle = (rand() % 360);
				model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

				// 4. 添加到矩阵的数组中
				rockModelMatrices[i] = model;

				//初始化旋转轴
				auto horizontalDir = glm::cross(glm::vec3(x, y, z), glm::vec3(0, 1, 0));
				glm::vec3 axis = glm::cross(glm::vec3(x, y, z), horizontalDir);
				axis = glm::normalize(axis);
				asteroidRevolvesAxis[i] = axis;
			}
			//修改VertexBuffer数据
			auto vb = rockModel->meshes[0].va->vbList[1]; //所有mesh的实例化属性共用同一个vb
			vb->SetData(&rockModelMatrices[0], sizeof(glm::mat4) * asteroidBeltAmount, 0);
		}
	};
}

