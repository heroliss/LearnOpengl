#include "test.h"
#include "imgui.h"

#include "TestClearColor.h"
#include "TestTexture2D.h"
#include "Test3D.h"
#include "Application.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

namespace test
{
	TestMenu::TestMenu() : m_CurrentTest(nullptr)
	{
		RegisterTest<TestClearColor>("Clear Color");
		RegisterTest<TestTexture2D>("Texture 2D");
		RegisterTest<Test3D>("3D Cube");
	}

	TestMenu::~TestMenu()
	{
		if (m_CurrentTest != nullptr) delete m_CurrentTest;
	}

	void TestMenu::OnUpdate(float deltaTime)
	{
		if (m_CurrentTest)
		{
			m_CurrentTest->OnUpdate(deltaTime);
		}
	}

	void TestMenu::OnRender()
	{
		if (m_CurrentTest)
		{
			m_CurrentTest->OnRender();
		}
	}

	void TestMenu::OnImGuiRender()
	{
		auto app = Application::GetInstance();
		auto& renderer = app->renderer;
		if (ImGui::Begin("##Inspector"))
		{
			//显示帧率
			float passTime = app->Time - lastTime;
			if (passTime >= 0.5f) {
				fps = (app->frameCount - lastFrameCount) / passTime;
				lastTime = app->Time;
				lastFrameCount = app->frameCount;
			}
			ImGui::Text("FPS %.1f  Drawcall:%d  Clear:%d  PostProcess:%d", fps, renderer->drawCallCount, renderer->clearCount, renderer->postProcessCount);
			//目标帧率
			ImGui::SameLine();
			ImGui::SetNextItemWidth(40);
			ImGui::DragFloat("TargetFPS", &app->TargetFrameRate, 1, 1, 9999, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
			//全屏模式
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100.0f);
			if (ImGui::Checkbox("Fullscreen", &fullscreenMode)) {
				app->ResetWindow(fullscreenMode ? glfwGetPrimaryMonitor() : nullptr);
			}

			//渲染图元数量统计
			ImGui::Text("Vertices:%d  Triangles:%d  LineSegments:%d  Points:%d  Quads:%d",
				renderer->primitiveCountInfo.numVertices,
				renderer->primitiveCountInfo.numTriangles,
				renderer->primitiveCountInfo.numLineSegments,
				renderer->primitiveCountInfo.numPoints,
				renderer->primitiveCountInfo.numQuads);

			//修改宽高
			bool screenChanged = false;
			ImGui::SetNextItemWidth(50);
			screenChanged = ImGui::DragInt("X", (int*)&renderer->ViewportWidth, 1, 1, 99999, "%d", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50);
			screenChanged |= ImGui::DragInt("Viewport Size", (int*)&renderer->ViewportHeight, 1, 1, 99999, "%d", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
			if (screenChanged) {
				app->renderer->ResetViewportSize(renderer->ViewportWidth, renderer->ViewportHeight);
			}
			ImGui::SameLine();
			ImGui::Checkbox("auto resize", &app->AutoResizeViewportByWindow);

			if (app->renderer->usingDebugShowLayer)
			{
				ImGui::SameLine();
				ImGui::SetNextItemWidth(100);
				ImGui::SliderInt("layer", &app->renderer->debugShowLayer, 0, 6, "%d", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
			}

			if (m_CurrentTest)
			{
				//返回按钮
				if (ImGui::Button("<-"))
				{
					delete m_CurrentTest;
					m_CurrentTest = nullptr;
					//恢复数据
					app->renderer->ResetCamera();
					app->renderer->ClearLights();
				}
				else
				{
					if (enableStepLimit == false) {
						app->renderer->stepCountLimit = app->renderer->stepCount;
					}
					//显示drawcall滑动条
					ImGui::SameLine(0, 20);
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 210.0f);
					ImGui::SliderInt(std::format("Step({})", app->renderer->stepCount).c_str(), &app->renderer->stepCountLimit, 0, app->renderer->stepCount);
					ImGui::SameLine(0);
					ImGui::Checkbox("lock", &enableStepLimit);
					//渲染当前GUI
					m_CurrentTest->OnImGuiRender();
				}
			}
			else
			{
				//所有功能按钮
				for (auto& test : m_Tests)
				{
					if (ImGui::Button(test.first.c_str()))
					{
						m_CurrentTest = test.second();
						//重置一些公用参数
						renderer->ResetCamera();
						app->input->ModelMatrix = glm::mat4(1.0f);
					}
				}
			}
		}
		ImGui::End();
	}
}
