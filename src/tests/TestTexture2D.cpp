#include "TestTexture2D.h"
#include "imgui.h"
#include "Application.h"
#include "VertexBufferLayout.h"
#include "../res/Materials/BaseMaterial_Blue.h" //载入材质的象征性做法
#include "glm/gtx/transform.hpp"

namespace test {

	TestTexture2D::TestTexture2D()
	{
		auto app = Application::GetInstance();

		//初始化顶点数据
		float positions[] =
		{
			//二维坐标    纹理坐标     颜色                    纹理索引
			-0.5f, -0.5f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1,
			 0.5f, -0.5f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1,
			 0.5f,  0.5f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1,
			-0.5f,  0.5f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 1,
		};


		unsigned int indices[] =
		{
			0,1,2,
			2,3,0
		};

		//带颜色的矩形
		float positions2[] =
		{
			//三维坐标        纹理坐标     颜色                    纹理索引
			-0.2f, -0.2f, 10, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 2.0f, 0,
			 0.2f, -0.2f, 10, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0,
			 0.2f,  0.2f, 10, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0,
			-0.2f,  0.2f, 10, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0,
		};


		va = std::make_unique<VertexArray>();
		auto vb = std::make_shared<VertexBuffer>(positions, sizeof(positions), sizeof(positions) / sizeof(positions[0]));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		layout.Push<float>(4);
		layout.Push<float>(1);
		va->AddBuffer(vb, layout);

		va2 = std::make_unique<VertexArray>();
		auto vb2 = std::make_shared<VertexBuffer>(positions2, sizeof(positions2), sizeof(positions2) / sizeof(positions2[0]));
		va2->AddBuffer(vb2, layout);

		ib = std::make_unique<IndexBuffer>(indices, sizeof(indices));

		//初始化材质
		auto m = std::make_unique<BaseMaterial_Blue>();
		material = std::move(m);

		/*va->UnBind();
		vb->Unbind();
		ib->Unbind();*/
	}

	TestTexture2D::~TestTexture2D()
	{
		std::cout << "销毁TestTexture2D！！！！！！！！！！！！" << std::endl;
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
		//不停的旋转
		//auto mm = Application::GetInstance()->renderer->ModelMatrix;
		//mm = glm::rotate(mm, deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
		//Application::GetInstance()->renderer->ModelMatrix = mm;
	}

	void TestTexture2D::OnRender()
	{
		auto app = Application::GetInstance();
		auto renderer = app->renderer.get();
		float scale = 600;
		glm::mat4 modelMatrix = glm::scale(app->input->ModelMatrix, glm::vec3(scale));
		renderer->Draw(*va, *material, modelMatrix, ib.get());
		renderer->Draw(*va2, *material, modelMatrix, ib.get());
	}

	void TestTexture2D::OnImGuiRender()
	{
	}
}
