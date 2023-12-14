#include "Test3D.h"
#include "imgui.h"
#include "Application.h"
#include "VertexBufferLayout.h"
#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"
#include <algorithm>

namespace test {
	Test3D::Test3D()
	{
		auto app = Application::GetInstance();

		float positions[] = {
			//位置                法线                 uv    Tangent  Bitangent
			//每个面四个点，从左下开始逆时针            	        
			//正面										          
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0,0,  1, 0, 0,  0,1,0,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1,0,  1, 0, 0,  0,1,0,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1,1,  1, 0, 0,  0,1,0,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0,1,	 1, 0, 0,  0,1,0,
			//背面										                           
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0,0,  -1, 0, 0,  0,1,0,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1,0,  -1, 0, 0,  0,1,0,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1,1,  -1, 0, 0,  0,1,0,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0,1,  -1, 0, 0,  0,1,0,
			 //左面										                           
			 -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0,0,   0, 0, 1,  0,1,0,
			 -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1,0,   0, 0, 1,  0,1,0,
			 -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1,1,   0, 0, 1,  0,1,0,
			 -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0,1,   0, 0, 1,  0,1,0,
			 //右面										                           
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0,0,   0, 0, -1,  0,1,0,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1,0,   0, 0, -1,  0,1,0,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1,1,   0, 0, -1,  0,1,0,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0,1,   0, 0, -1,  0,1,0,
			 //底面										                           
			 -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0,0,   1, 0, 0,  0,0,1,
			  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1,0,   1, 0, 0,  0,0,1,
			  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1,1,   1, 0, 0,  0,0,1,
			 -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0,1,   1, 0, 0,  0,0,1,
			 //顶面										                           
			 -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0,0,   1, 0, 0,  0,0,-1,
			  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1,0,   1, 0, 0,  0,0,-1,
			  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1,1,   1, 0, 0,  0,0,-1,
			 -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0,1,   1, 0, 0,  0,0,-1,
		};

		// 顶点索引数组	
		unsigned int indices[] = {
			0, 1, 2, // 正面
			2, 3, 0,
			4, 5, 6, // 背面
			6, 7, 4,
			8, 9, 10, // 左面
			10, 11, 8,
			12, 13, 14, // 右面
			14, 15, 12,
			16, 17, 18, // 底面
			18, 19, 16,
			20, 21, 22, // 顶面
			22, 23, 20,
		};

		va = std::make_unique<VertexArray>();
		auto vb = std::make_shared<VertexBuffer>(VertexBufferParams_array(positions));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);
		layout.Push<float>(3);
		//layout.Push<float>(3);

		va->AddBuffer(vb, layout);

		ib = std::make_unique<IndexBuffer>(indices, sizeof(indices));

		//中心点va
		va_centerPoint = std::make_unique<VertexArray>();
		auto vb_centerPoint = std::make_shared<VertexBuffer>(nullptr, 0, 0); //这里不填数据也可以？
		VertexBufferLayout centerlayout;
		va_centerPoint->AddBuffer(vb_centerPoint, centerlayout);

		//初始化材质
		mainMaterial = std::make_unique<BaseMaterial3D>();
		normalDisplayMaterial = std::make_unique<NormalDisplayMaterial>();		//显示法线的材质
		lightDisplayMaterial = std::make_unique<LightDisplayMaterial>();		//显示光源信息的材质
		singleColorMaterial = std::make_unique<SingleColorMaterial>();
		texcoordDisplayMaterial = std::make_unique<TexcoordDisplayMaterial>();

		/*va->UnBind();
		vb->Unbind();
		ib->Unbind();*/
	}

	Test3D::~Test3D()
	{
	}

	void Test3D::OnUpdate(float deltaTime)
	{

	}

	void Test3D::AddToDrawList(const VertexArray& va, std::shared_ptr<Material> material, glm::mat4 modelMatrix, const IndexBuffer* ib, const unsigned int instanceCount, unsigned int mode, const unsigned int count)
	{
		DrawInfo info = DrawInfo(&va, material, modelMatrix, ib, mode, count, instanceCount);
		if (material->IsTransparent)
		{
			glm::vec3 cameraToModel = glm::vec3(modelMatrix * glm::vec4(0, 0, 0, 1)) - Application::GetInstance()->renderer->camera.position;
			float distance = glm::length(cameraToModel);
			//Application::GetInstance()->renderer->transparentDrawMap[distance] = info;

			info.depth = distance;
			Application::GetInstance()->renderer->transparentDrawList.push_back(info);
		}
		else {
			Application::GetInstance()->renderer->opaqueDrawList.push_back(info);
		}
		Application::GetInstance()->renderer->combineDrawList.push_back(info);
	}



	void Test3D::OnRender()
	{
		auto app = Application::GetInstance();

		//清空绘制列表
		//app->renderer->transparentDrawMap.clear();
		app->renderer->transparentDrawList.clear();
		app->renderer->opaqueDrawList.clear();
		app->renderer->combineDrawList.clear();

		//添加立方体绘制
		if (currentModelIndex == 0)
		{
			//设置立方体的贴图
			mainMaterial->MainTexture = enableMainTexture ? Texture::Get("res/textures/brickwall.jpg") : Texture::Get(255, 255, 255);
			mainMaterial->NormalTexture = enableNormalTexture ? Texture::Get("res/textures/brickwall_normal.jpg") : Texture::Get(128, 128, 255);
			mainMaterial->SpecularTexture = enableSpecularTexture ? Texture::Get("res/textures/dog.jpg") : Texture::Get(255, 255, 255);
			mainMaterial->HeightTexture = nullptr;
			mainMaterial->cubemap = showSkybox ? skybox.GetCubemap() : nullptr; //设置环境立方体贴图
			//设置是否显示深度
			mainMaterial->showDepth = showDepth;
			mainMaterial->showDepthRange = showDepthRange;
			//设置模型矩阵
			glm::mat4 modelMatrix = GetCurrentModelMatrix();//中心立方体的旋转由input控制
			//添加中心立方体绘制
			AddToDrawList(*va, mainMaterial, modelMatrix, ib.get());
			//渲染多个立方体
			if (mutiCubes) {
				srand(randomSeed); //重启随机数列表，保证下面的rand每次随机到一样的结果
				for (int i = 0; i < 20; i++)
				{
					//每个立方体单独旋转
					//glm::mat4 m = glm::translate(glm::mat4(1), glm::vec3((std::rand() % 6 - 3)*modelScales[currentModelIndex], (std::rand() % 6 - 3) * modelScales[currentModelIndex], (std::rand() % 6 - 3) * modelScales[currentModelIndex]));
					//auto otherModelMatrix = m * modelMatrix;
					//所有立方体一起旋转
					auto otherModelMatrix = glm::translate(modelMatrix, glm::vec3((std::rand() % 6 - 3), (std::rand() % 6 - 3), (std::rand() % 6 - 3)));
					//添加其他立方体绘制
					AddToDrawList(*va, mainMaterial, otherModelMatrix, ib.get());
				}
			}
		}
		//添加模型绘制
		else
		{
			auto model = Model::Get(modelPaths[currentModelIndex]);
			glm::mat4 modelMatrix = GetCurrentModelMatrix(); //设置大小和偏移
			//对模型的每一个mesh加入到绘制列表
			for (int j = 0; j < model->meshes.size(); j++)
			{
				auto mesh = &model->meshes[j];
				auto material = std::make_shared<BaseMaterial3D>(); //每帧新建材质(可优化)，每个mesh可能有不同的贴图设置
				//从网格信息中设置贴图和颜色
				material->SetFromAiMaterial(mesh->mat, model->directory, enableNormalTexture, enableMainTexture, enableSpecularTexture);
				SetMaterialSameAsMainMaterial(material);
				AddToDrawList(*mesh->va, material, modelMatrix, mesh->ib.get());
			}

			//对于第六个行星模型，额外绘制小行星带
			if (currentModelIndex == 5) {
				if (rockModel == nullptr)
					InitAsteroid();
				for (int j = 0; j < rockModel->meshes.size(); j++)
				{
					auto mesh = &rockModel->meshes[j];
					auto material = std::make_shared<BaseMaterial3D>(); //每帧新建材质(可优化)，每个mesh可能有不同的贴图设置
					//从网格信息中设置贴图和颜色
					material->SetFromAiMaterial(mesh->mat, rockModel->directory, enableNormalTexture, enableMainTexture, enableSpecularTexture);
					SetMaterialSameAsMainMaterial(material);
					AddToDrawList(*mesh->va, material, glm::mat4(0), mesh->ib.get(), asteroidBeltAmount);
				}
				//小行星公转
				AsteroidRevolves();
			}
		}

		//------------开始绘制列表--------------
		app->renderer->SetDepthTestActive(true); //开启深度测试
		app->renderer->SetPolygonFillMode(); //填充模式

		if (showOutline)
		{
			//启用模板测试
			GLCALL(glEnable(GL_STENCIL_TEST));
			//设置模板测试和深度测试都通过后总是写入1
			GLCALL(glStencilFunc(GL_ALWAYS, 1, 0xFF));
			GLCALL(glStencilMask(0xFF));
			GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
		}
		else {
			//关闭模板测试
			GLCALL(glDisable(GL_STENCIL_TEST));
		}

		//非透视模式显示线框或点时，先写入一遍深度，来丢弃被挡住的片元
		if (canSeeThrough == false && (showLine || showPoint))
		{
			//先用Fill模式写入深度，这样做目的是在线框或点模式中依然有整个面的深度信息，来丢弃背面被挡住的像素
			//app->renderer->SetColorDiscard(true); //丢弃渲染结果，不写入颜色缓存 （或用下面的清除颜色缓存，这样可以在步骤调试时看到渲染结果）
			for (int i = 0; i < app->renderer->opaqueDrawList.size(); i++)
			{
				auto& info = app->renderer->opaqueDrawList[i];
				app->renderer->Draw(*info.va, *singleColorMaterial, info.modelMatrix, info.ib, info.instanceCount, info.mode, info.count);
			}
			//app->renderer->SetColorDiscard(false); //恢复丢弃渲染结果
			app->renderer->ClearColorBuffer();
		}

		//设置线框或点模式
		if (showLine) app->renderer->SetPolygonLineMode();
		else if (showPoint) app->renderer->SetPolygonPointMode();

		//渲染物体的方法
		auto drawObject = [=](DrawInfo& info)
			{
				if (showTexcoord) app->renderer->Draw(*info.va, *texcoordDisplayMaterial, info.modelMatrix, info.ib, info.instanceCount, info.mode, info.count); //渲染纹理坐标
				else app->renderer->Draw(*info.va, *info.material, info.modelMatrix, info.ib, info.instanceCount, info.mode, info.count); //渲染物体
			};

		//渲染不透明物体
		for (int i = 0; i < app->renderer->opaqueDrawList.size(); i++)
		{
			auto& info = app->renderer->opaqueDrawList[i];
			drawObject(info);
		}

		//绘制天空盒
		if (showSkybox)
		{
			GLCALL(glStencilMask(0x00)); //不写入模板
			skybox.Draw();
			GLCALL(glStencilMask(0xFF)); //恢复写入模板
		}

		//排序透明物体
		auto customSort = [app](const DrawInfo& a, const DrawInfo& b) {
			return a.depth > b.depth;
			};
		std::sort(app->renderer->transparentDrawList.begin(), app->renderer->transparentDrawList.end(), customSort);

		//渲染透明物体
		for (int i = 0; i < app->renderer->transparentDrawList.size(); i++)
		{
			auto& info = app->renderer->transparentDrawList[i];
			drawObject(info);
		}

		//从map中逆序渲染透明物体
		//for (std::map<float, DrawInfo>::reverse_iterator it = app->renderer->transparentDrawMap.rbegin(); it != app->renderer->transparentDrawMap.rend(); ++it)
		//{
		//	drawObject(it->second);
		//}

		//恢复为填充模式
		app->renderer->SetPolygonFillMode();

		//绘制边框
		if (showOutline) {
			for (int i = 0; i < app->renderer->combineDrawList.size(); i++)
			{
				auto& info = app->renderer->combineDrawList[i];
				GLCALL(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
				app->renderer->Draw(*info.va, *singleColorMaterial, glm::scale(info.modelMatrix, glm::vec3(1.02f)), info.ib, info.instanceCount, info.mode, info.count);
			}
		}

		//绘制法线（透明）
		if (normalDisplayMaterial->showFragmentNormal || normalDisplayMaterial->showVertexNormal)
		{
			for (int i = 0; i < app->renderer->combineDrawList.size(); i++)
			{
				auto& info = app->renderer->combineDrawList[i];
				auto* mat = dynamic_cast<BaseMaterial3D*>(info.material.get());
				if (mat == nullptr) continue;
				normalDisplayMaterial->NormalTexture = mat->NormalTexture;
				app->renderer->Draw(*info.va, *normalDisplayMaterial, info.modelMatrix, info.ib, info.instanceCount, info.mode, info.count);
			}
		}
		//-----------------------------------------

		//光源（透明）
		for (int i = 0; i < app->renderer->lights.size(); i++)
		{
			auto& light = app->renderer->lights[i];
			//自动旋转光源
			if (light->autoRotate && light->autoRotateSpeed != 0)
			{
				auto rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(light->autoRotateSpeed) * app->DeltaTime, light->autoRotateAxis);
				light->pos = rotateMatrix * glm::vec4(light->pos, 1);
			}
			//光源方向恒指向中心
			if (light->autoLookAtCenter)
			{
				light->direction = glm::normalize(-light->pos);
			}
			//绘制光源图示
			if (light->showGizmo) {
				lightDisplayMaterial->lightIndex = i;
				lightDisplayMaterial->gizmoSize = light->gizmoSize;
				app->renderer->Draw(*va_centerPoint, *lightDisplayMaterial, glm::translate(glm::mat4(1.0f), light->pos), nullptr, 1, GL_POINTS, 1);
			}
		}
	}

	ImVec4 titleColor = ImVec4(61 / 255.0f, 133 / 255.0f, 224 / 255.0f, 1);
	void Test3D::OnImGuiRender()
	{
		auto app = Application::GetInstance();

		//------------------------模型---------------------------
		if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
		{
			//模型选择
			ImGui::SetNextItemWidth(100);
			ImGui::Combo("##empty", &currentModelIndex, modelNames, IM_ARRAYSIZE(modelNames));
			//立方体模型设置
			if (currentModelIndex == 0)
			{
				ImGui::SameLine();
				ImGui::Checkbox("Multiple cubes", &mutiCubes);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(50);
				ImGui::DragInt("Random Seed", &randomSeed);
			}
			//小行星带模型设置
			else if (currentModelIndex == 5)
			{
				ImGui::SameLine();
				ImGui::SetNextItemWidth(60);
				if (ImGui::DragInt("Asteroid Amount", (int*)&asteroidBeltAmount, 1, 0, 999999))
				{
					for (int j = 0; j < rockModel->meshes.size(); j++)
					{
						auto mesh = &rockModel->meshes[j];
						if (mesh->va->vbList.size() > 1)
						{
							//小行星数量发生变化，需要重新生成石块的实例化vertexBuffer
							mesh->va->PopVertexBuffer(); //先移除旧的vb
							InitAsteroid();
						}
					}
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(60);
				if (ImGui::DragInt("Random Seed", &randomSeed)) InitAsteroidPos(); //重新计算和设置实例化vertexbuffer中的位置数据

				ImGui::SetNextItemWidth(60);
				ImGui::DragFloat("Planet Mass(mt)", &planetMass, 0.1f);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(60);
				ImGui::DragFloat("Time Scale", &planetTimeScale, 10.0f);

				ImGui::SetNextItemWidth(60);
				if (ImGui::DragFloat("Radius(m)", &asteroidBeltRadius, 1.0f, 100.0f)) InitAsteroidPos();
				ImGui::SetItemTooltip("小行星带最内圈半径");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(60);
				if (ImGui::DragFloat("Width(m)", &asteroidBeltWidth, 1.0f, 0.0f)) InitAsteroidPos();
				ImGui::SetItemTooltip("小行星带宽度（既xz方向向外拓展的距离）");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(60);
				if (ImGui::DragFloat("Thickness(m)", &asteroidBeltThickness, 1.0f, 0.0f)) InitAsteroidPos();
				ImGui::SetItemTooltip("小行星带厚度（既y方向的厚度）");
			}
		}

		//----------------------------渲染---------------------------------------
		if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
		{
			ImGui::Checkbox("Show Skybox", &showSkybox);
			ImGui::SameLine();
			ImGui::Checkbox("Show outline", &showOutline);
			ImGui::SameLine();

			//MSAA设置
			int logMassSamples = glm::log2((float)app->renderer->multiSample);
			//窗口开启了多采样并且没有后处理过程时（提前的自定义帧缓冲多采样结果会直接位块传送到默认帧缓冲），此时提前的自定义帧缓冲多采样数必须与默认帧缓冲采样数相等，或者默认帧缓冲不使用多重采样
			//简单说就是两个多采样帧缓冲之间位块传送时必须采样数相等（若是一个多采样向另一个正常帧缓冲位块传送就没有限制）
			if (app->WindowMsaaSamples > 0 && app->renderer->HasPostProcessingToDraw() == false)
			{
				ImGui::Text("MSAA x%d", app->WindowMsaaSamples);
			}
			else
			{
				ImGui::SetNextItemWidth(120);
				if (ImGui::SliderInt("MSAA", &logMassSamples, 0, 4, ""))
				{
					app->renderer->SetMultiSample(logMassSamples == 0 ? 0 : glm::pow(2, logMassSamples));
				}
				ImGui::SameLine();
				ImGui::Text(("x" + std::to_string(app->renderer->multiSample)).c_str());
			}
		}

		//----------------------------调试---------------------------------------
		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
		{
			ImGui::Checkbox("Show texcoord", &showTexcoord);

			ImGui::Checkbox("Show vertex normal", &normalDisplayMaterial->showVertexNormal);
			ImGui::SameLine();
			ImGui::Checkbox("Show fragment normal", &normalDisplayMaterial->showFragmentNormal);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("Size", &normalDisplayMaterial->size, 0.1f);

			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("Samples Interval", &normalDisplayMaterial->showFragmentNormalInterval, 0.01f);
			ImGui::SetItemTooltip("This is the sampling interval of the fragment normal map.\nif the value is 0 or negative, then the number of samples specified by the [sample size] will be used.");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			ImGui::DragFloat2("Samples Size", &normalDisplayMaterial->showFragmentNormalSamplesSize.x, 0.01f);

			if (ImGui::Checkbox("Show line", &showLine))
			{
				if (showLine)
				{
					showPoint = false;
				}
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Show point", &showPoint))
			{
				if (showPoint)
				{
					showLine = false;
				}
			}
			ImGui::SameLine();
			ImGui::Checkbox("Can See Through", &canSeeThrough);

			ImGui::Checkbox("Show depth", &showDepth);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(160);
			if (ImGui::DragFloat2("Range", &showDepthRange.x, 1.0f, 1.0f))
			{
				if (showDepthRange.x < 1) showDepthRange.x = 1;
				if (showDepthRange.y < 1) showDepthRange.y = 1;
				if (showDepthRange.x > showDepthRange.y) showDepthRange.x = showDepthRange.y;
			}

			ImGui::SetNextItemWidth(100);
			if (ImGui::DragFloat2("Polygon Offset Factor", &app->renderer->polygonOffsetFactor.x))
			{
				app->renderer->ApplyPolygonOffsetFactor();
			}
		}

		//-------------------------相机--------------------------------
		app->renderer->camera.OnInspectorGUI();

		//--------------------------材质--------------------------------
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
		{
			//透明开关
			ImGui::Checkbox("Is Transparent", &mainMaterial->IsTransparent);
			ImGui::SetItemTooltip("标记是透明物体（仅影响渲染顺序，透明物体在 不透明物体 和 天空盒 之后，且排序后才渲染）");

			//贴图开关
			ImGui::Checkbox("Main Texture", &enableMainTexture);
			ImGui::SameLine();
			ImGui::Checkbox("Specular Texture", &enableSpecularTexture);
			ImGui::SameLine();
			ImGui::Checkbox("Normal Texture", &enableNormalTexture);

			//物体颜色
			ImGui::SetNextItemWidth(200);
			ImGui::ColorEdit4("object color", &mainMaterial->ObjectColor.x);

			//环境光颜色
			ImGui::SetNextItemWidth(200);
			ImGui::ColorEdit3("attenuation color", &mainMaterial->Ambient.x);

			//内部自发光颜色
			ImGui::SetNextItemWidth(200);
			ImGui::ColorEdit3("inside emission color", &mainMaterial->Emission_inside.x);

			//外部自发光颜色
			ImGui::SetNextItemWidth(200);
			ImGui::ColorEdit3("outside emission color", &mainMaterial->Emission_outside.x);

			//高光反射颜色
			ImGui::SetNextItemWidth(200);
			ImGui::ColorEdit3("specular color", &mainMaterial->SpecularColor.x);

			//高光反射范围（光泽度）
			ImGui::SameLine(0, 20);
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("shininess", &mainMaterial->Shininess);

			//折射开关
			ImGui::Checkbox("Enable refract skybox", &mainMaterial->enableRefract);
			ImGui::SetItemTooltip("开启折射（目前仅支持折射天空盒，不会折射其他物体）");

			//折射率
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("refractive Index", &mainMaterial->refractiveIndex, 0.001f);
			ImGui::SetItemTooltip("折射率 (空气1.00  水1.33  冰1.309  玻璃1.52  钻石2.42)");

			//折射颜色
			ImGui::SetNextItemWidth(200);
			ImGui::ColorEdit3("refract Color", &mainMaterial->refractColor.x);
		}


		//TODO: 视角旋转到90度时会卡住

		//---------------------光照--------------------------
		if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
		{
			ImGui::NewLine();
			for (int i = 0; i < lights.size(); i++)
			{
				//auto typeName = Light::LightTypeToString(light.type) + id;
				bool isSelected = selectedItemIndex == i;
				ImGui::SameLine();
				if (ImGui::RadioButton(std::to_string(i).c_str(), isSelected))
				{
					selectedItemIndex = i;
				}
			}
			if (lights.size() < MAX_LIGHT_COUNT)
			{
				ImGui::SameLine(ImGui::GetWindowWidth() - 90.0f);
				if (ImGui::Button("Add light"))
				{
					Application::GetInstance()->renderer->AddLight();
				}
			}
			if (selectedItemIndex >= 0)
			{
				Light& light = *lights[selectedItemIndex];
				std::string id = "##" + std::to_string(selectedItemIndex);
				//光源类型
				static const char* items[]{ "Disable", "parallel light","point light","spot light" };
				int selecteditem = light.type;
				ImGui::SetNextItemWidth(200);
				if (ImGui::Combo(("Light " + id).c_str(), &selecteditem, items, IM_ARRAYSIZE(items)))
				{
					light.type = LightType(selecteditem);
				}

				//删除光源
				ImGui::SameLine(ImGui::GetWindowWidth() - 70.0f);
				if (ImGui::Button(("Delete" + id).c_str()))
				{
					Application::GetInstance()->renderer->RemoveLight(selectedItemIndex);
					selectedItemIndex = -1;
				}

				//光源位置
				ImGui::SetNextItemWidth(200);
				ImGui::DragFloat3(("position" + id).c_str(), &light.pos.x);

				//光照方向(显示为欧拉角角度数, 但存储为方向向量，所以需要转换)  TODO: 欧拉角旋转有问题
				glm::vec3 originalDirection = glm::vec3(0, 0, -1);
				glm::vec3 targetDirection = glm::normalize(light.direction);
				float dotProduct = glm::dot(originalDirection, targetDirection);
				float angle = glm::acos(dotProduct);
				glm::vec3 eulerAngle = glm::vec3(0);
				if (angle != 0)
				{
					//下面rotationAxis如果为0，则会出错，因为旋转轴方向不存在了，因此需要排除角度为0的情况
					glm::vec3 rotationAxis = glm::cross(originalDirection, targetDirection);
					rotationAxis = glm::normalize(rotationAxis);
					glm::quat rotation = glm::angleAxis(angle, rotationAxis);
					eulerAngle = glm::degrees(glm::eulerAngles(rotation));
				}
				ImGui::SetNextItemWidth(200);
				if (ImGui::DragFloat3(("direction" + id).c_str(), &eulerAngle.x))
				{
					eulerAngle = glm::radians(eulerAngle); //转换回弧度
					glm::mat4 rotationMatrix = glm::eulerAngleXYZ(eulerAngle.x, eulerAngle.y, eulerAngle.z);
					light.direction = rotationMatrix * glm::vec4(originalDirection, 0);
				}

				//光源颜色
				ImGui::SetNextItemWidth(200);
				ImGui::ColorEdit3(("color" + id).c_str(), &light.color.x);
				ImGui::SetNextItemWidth(100);

				//光源亮度
				ImGui::SameLine(0, 20);
				ImGui::SetNextItemWidth(50);
				ImGui::DragFloat(("brightness" + id).c_str(), &light.brightness, 0.01f);

				//衰减系数
				ImGui::SetNextItemWidth(200);
				ImGui::DragFloat3(("attenuation" + id).c_str(), &light.attenuation.x, 0.00001f, 0, 0, "%f");

				//聚光范围
				if (light.type == LightType::SPOT_LIGHT)
				{
					ImGui::SetNextItemWidth(200);
					ImGui::DragFloat2(("cutoffAngle(inner/outer)" + id).c_str(), &light.cutoffAngle.x);
				}

				ImGui::Spacing();

				//gizmo
				ImGui::Checkbox(("Show gizmo" + id).c_str(), &light.showGizmo);
				ImGui::SameLine(0, 20);
				ImGui::SetNextItemWidth(60);
				ImGui::DragFloat(("Gizmo size" + id).c_str(), &light.gizmoSize, 0.1f);
				//光源自动旋转
				ImGui::Checkbox(("Auto rotate" + id).c_str(), &light.autoRotate);
				ImGui::SameLine(0, 20);
				ImGui::SetNextItemWidth(60);
				ImGui::DragFloat(("Speed (angle/s)" + id).c_str(), &light.autoRotateSpeed, 1, 0);
				ImGui::SetNextItemWidth(200);
				if (ImGui::DragFloat3(("Auto rotate axis" + id).c_str(), &light.autoRotateAxisEulerAngles.x))
				{
					light.UpdateAutoRotateAxisByEulerAngles();
				}
				ImGui::Checkbox(("Keep looking at center" + id).c_str(), &light.autoLookAtCenter);
				ImGui::Checkbox(("Use Blinn-Phong" + id).c_str(), &light.useBlinnPhong);
			}
		}

		//-----------------------后处理------------------------------
		bool postProcessDirty = false;
		if (ImGui::CollapsingHeader("Post processing", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow))
		{
			ImGui::SetItemTooltip("You can drag and drop to sort");
			if (ImGui::Checkbox("Enable##Post-processing", &app->renderer->enablePostProcessing))
			{
				postProcessDirty = true;
			}
			ImGui::SameLine(ImGui::GetWindowWidth() - 160.0f);
			if (ImGui::BeginCombo("##AddMaterial", "Add Post-Processing", ImGuiComboFlags_WidthFitPreview | ImGuiComboFlags_NoArrowButton))
			{
				for (int i = 0; i < postProcessMaterials.size(); i++)
				{
					auto material = postProcessMaterials[i];
					auto name = material->GetName();
					if (ImGui::Button((name + "##id_" + std::to_string(i)).c_str()))
					{
						auto ptr = std::shared_ptr<PostProcessingMaterial>(material->CreateObject());
						app->renderer->postProcessingMaterials.push_back(ptr);
						postProcessDirty = true;
					}
				}
				ImGui::EndCombo();
			}

			for (int i = 0; i < app->renderer->postProcessingMaterials.size(); i++)
			{
				auto material = app->renderer->postProcessingMaterials[i];
				std::string idText = "##Item " + std::to_string(i);
				if (ImGui::Button(("X" + idText).c_str()))
				{
					app->renderer->postProcessingMaterials.erase(app->renderer->postProcessingMaterials.begin() + i); //删除一个后处理材质
					postProcessDirty = true;
					break;
				}

				ImGui::SameLine();
				ImGui::Text(material->GetName().c_str());

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					int index = i;
					ImGui::SetDragDropPayload("DragMaterial", &index, sizeof(index));
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget()) {
					const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragMaterial");
					if (payload) {
						int index = *static_cast<const int*>(payload->Data);
						if (index != i) {
							std::swap(app->renderer->postProcessingMaterials[index], app->renderer->postProcessingMaterials[i]);
							postProcessDirty = true;
						}
					}
					ImGui::EndDragDropTarget();
				}

				material->DrawImgui(i);
			}
		}

		//后处理发生变化后，如果窗口多采样已开启，并且没有后处理过程，则需要关闭预先多采样，以防止预先多采样和窗口多采样采样数不一致导致的崩溃问题
		if (postProcessDirty && app->WindowMsaaSamples > 0 && app->renderer->HasPostProcessingToDraw() == false)
		{
			app->renderer->SetMultiSample(0);
		}
	}
}