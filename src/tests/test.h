#pragma once

#include <functional>
#include <vector>
#include <string>
#include <iostream>

namespace test {

	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu();
		~TestMenu() override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Test* m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;

		template<typename T>
		void RegisterTest(const std::string& name)
		{
			std::cout << "Registering test " << name << std::endl;
			m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
		}

		//用于计算帧率
		float lastTime = 0;
		int lastFrameCount = 0;
		float fps = 0;

		bool fullscreenMode;
		bool enableStepLimit;
	};
}

