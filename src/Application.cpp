#include "Application.h"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "renderer.h"

#include "glm/gtc/matrix_transform.hpp"

#include "GUI.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "Input.h"

#include "Model.h"
#include "Material.h"

Application* Application::m_instance;
Application::Application()
	: renderer(nullptr), window(nullptr), input(nullptr), videoMode()
{
	Application::m_instance = this;
}

Application::~Application()
{
	if (Application::m_instance == this) {
		Application::m_instance = nullptr;
	}
}

// 窗口大小变化时的回调函数
void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	auto& renderer = Application::GetInstance()->renderer;
	renderer->WindowWidth = width;
	renderer->WindowHeight = height;
	if (Application::GetInstance()->AutoResizeViewportByWindow) {
		renderer->ResetViewportSize(width, height);
	}
}


int Application::Run()
{
start:

	{ //这个作用域用来保证结束时释放内存
		//初始化GLFW
		if (!glfwInit())
			return -1;
		const char* glsl_version = "#version 450";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
		glfwWindowHint(GLFW_SAMPLES, WindowMsaaSamples);  //设置MSAA抗锯齿

		//获取主显示器的分辨率，确定要使用的分辨率
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();	// 获取主显示器的句柄
		int initWidth = 800;
		int initHeight = 600;
		videoMode = glfwGetVideoMode(primaryMonitor);

		if (videoMode)
		{
			std::cout << "当前屏幕分辨率：" << videoMode->width << "x" << videoMode->height << std::endl;
			initWidth = videoMode->width / 2;
			initHeight = videoMode->height / 2;
		}
		else
		{
			std::cout << "无法获取屏幕分辨率" << std::endl;
		}

		//创建窗口
		window = glfwCreateWindow(initWidth, initHeight, "Hello World", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			return -1;
		}
		glfwMakeContextCurrent(window); //Make the window's context current
		glfwSwapInterval(1); //开启垂直同步
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //设置窗口大小改变回调
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//隐藏鼠标

		//初始化输入
		input = std::make_shared<Input>(window);

		//初始化Renderer
		renderer = std::make_shared<Renderer>(initWidth, initHeight, initWidth, initHeight);
		int err = renderer->Init();
		if (err != 0) return err;

		//初始化GUI
		GUI gui(glsl_version, window);
		test::TestMenu testMenu; //初始化启动弹窗

		//记录程序开始时间
		auto appStartTime = std::chrono::high_resolution_clock::now();
		std::chrono::steady_clock::time_point frameStartTime = appStartTime;

		while (!glfwWindowShouldClose(window))
		{
			//限制帧率
			auto frameEndTime = std::chrono::high_resolution_clock::now();
			auto frameTime = std::chrono::duration<double>(frameEndTime - frameStartTime).count();
			double targetFrameTime = 1.0 / TargetFrameRate;
			double remainingTime = targetFrameTime - frameTime;
			if (remainingTime > 0)
			{
				std::this_thread::sleep_for(std::chrono::duration<double>(remainingTime));
			}
			//开始帧，更新时间
			frameStartTime = std::chrono::high_resolution_clock::now();
			float currentTime = std::chrono::duration<float>(frameStartTime - appStartTime).count();
			DeltaTime = currentTime - Time;
			Time = currentTime;

			//处理输入
			glfwPollEvents();
			input->processInput(window);

			//恢复viewport
			if (renderer->NeedReapplyCurrentViewportSize) {
				renderer->NeedReapplyCurrentViewportSize = false;
				renderer->ApplyViewportSize(renderer->ViewportWidth, renderer->ViewportHeight, false, false);
			}

			//判断是否渲染到自定义帧缓冲（必须先于重置统计数据执行，因为需要用到上一帧的stepCount）
			bool hasPostProcessing = renderer->HasPostProcessingToDraw();

			//重置统计数据 （必须先于下面的clear执行，因为需要stepCount清零）
			renderer->ResetStatisticData();

			//若需要后处理，则切换到自定义帧缓冲
			if (hasPostProcessing)
			{
				renderer->SwitchAnotherCustomFrameBuffer(); //绑定到自定义帧缓冲并clear
			}
			//否则切换到默认帧缓冲
			else
			{
				renderer->SwitchDefaultFrameBuffer(); //绑定到默认帧缓冲并clear
			}

			//若需要多重采样，则渲染到多重采样帧缓冲（虽然最终选择了多重采样帧缓冲，但上面的切换当前帧缓冲和clear依然要执行）
			if (renderer->multiSample > 0)
			{
				renderer->BindMultiSampleFrameBuffer(); //绑定到多重采样帧缓冲并clear
			}

			//执行逻辑
			testMenu.OnUpdate(DeltaTime);

			//渲染图像
			testMenu.OnRender();

			//若多重采样，则渲染结果位块传送到当前帧缓冲（自定义帧缓冲或默认帧缓冲）
			if (renderer->multiSample > 0)
			{
				renderer->BlitMultiSampleFramebufferToCurrentFrameBuffer();
			}

			//图像后处理
			if (hasPostProcessing)
			{
				renderer->DrawAllPostProcessing();
			}
			else if (renderer->enablePostProcessing) {
				renderer->stepCount += renderer->postProcessingMaterials.size();
			}

			//渲染UI
			gui.BeginFrame();
			testMenu.OnImGuiRender();
			//gui.ShowDemoWindow();
			gui.EndFrame();

			//交互缓冲区
			glfwSwapBuffers(window);

			//结束帧
			frameCount++;
			//auto endTime = std::chrono::high_resolution_clock::now();
			//auto totalTime = std::chrono::duration<double>(endTime - frameStartTime).count();
		}

		//清理内存
		//delete primaryMonitor; //不是new创建的不需要释放
		//delete videoMode; //不是new创建的不需要释放
		input = nullptr;
		renderer = nullptr;
	} //作用域结束时将自动清理栈内存

	//清理静态缓存
	Material::ClearCache();
	Model::ClearCache();
	Texture::ClearCache();

	//销毁窗口
	glfwDestroyWindow(window);
	glfwTerminate();  //结束程序前要先清理内存，否则可能导致glGetError循环不能停止而导致程序无法正常退出
	//delete window; //不是new创建的不需要释放
	window = nullptr;

	if (needRestart) {
		needRestart = false;
		goto start;
	}
	return 0;
}

int main(void)
{
#ifdef _WIN32
	// Windows操作系统
	system("chcp 65001"); //设置活动代码页为UTF-8。解决在cmd中输出中文乱码的问题
#elif __linux__
	// Linux操作系统
#elif __APPLE__
	// macOS操作系统
#endif

	Application app;
	return app.Run();
}