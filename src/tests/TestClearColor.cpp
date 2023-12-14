#include "TestClearColor.h"
#include "Application.h"
#include "imgui.h"

namespace test {
	TestClearColor::TestClearColor()
		: m_ClearColor{ 0.0f, 0.0f, 0.0f, 0.0f }
	{
		auto color = Application::GetInstance()->renderer->GetClearColor();
		for (int i = 0; i < 4; i++)
		{
			m_ClearColor[i] = color[i];
		}
	}

	TestClearColor::~TestClearColor()
	{
	}

	void TestClearColor::OnUpdate(float deltaTime)
	{
	}

	void TestClearColor::OnRender()
	{
	}

	void TestClearColor::OnImGuiRender()
	{
		auto app = Application::GetInstance();
		if (ImGui::ColorEdit4("Clear Color", m_ClearColor))
		{
			Application::GetInstance()->renderer->SetClearColor(m_ClearColor, 4);
		}

		int logMassSamples = glm::log2((float)app->WindowMsaaSamples);
		ImGui::SetNextItemWidth(200);
		if (ImGui::SliderInt("Window MSAA", &logMassSamples, 0, 4, ""))
		{
			app->WindowMsaaSamples = logMassSamples == 0 ? 0 : glm::pow(2, logMassSamples);
			app->needRestart = true;
		}
		ImGui::SameLine();
		ImGui::Text(("x" + std::to_string(app->WindowMsaaSamples)).c_str());

		if (app->needRestart) {
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "请关闭窗口重启以生效");
		}
	}
}
