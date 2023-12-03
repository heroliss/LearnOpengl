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
		if (ImGui::ColorEdit4("Clear Color", m_ClearColor))
		{
			Application::GetInstance()->renderer->SetClearColor(m_ClearColor, 4);
		}
	}
}
