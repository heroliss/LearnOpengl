#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include "Application.h"


#define STBIW_WINDOWS_UTF8
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#ifdef _WIN32
#include <Windows.h>
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../vendor/stb/stb_image_write.h"

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
		glfwWindowHint(GLFW_VISIBLE, HiddenWindow ? GLFW_FALSE : GLFW_TRUE);

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
		if (!CapturePath.empty()) { initWidth = 1600; initHeight = 900; }
		window = glfwCreateWindow(initWidth, initHeight, "LearnOpenGL - Rendering Lab", NULL, NULL);
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
		if (!ShowcasePreset.empty()) testMenu.SelectShowcase(ShowcasePreset);

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

			//执行逻辑（填充渲染列表）
			testMenu.OnUpdate(DeltaTime);

			//判断是否渲染到自定义帧缓冲（必须先于重置统计数据执行，因为需要用到上一帧的stepCount）
			bool hasPostProcessing = renderer->HasPostProcessingToDraw();

			//重置统计数据 （必须先于下面的clear执行，因为需要stepCount清零）
			renderer->ResetStatisticData();

			//为每一个灯光渲染阴影贴图
			bool displayShadowMap = renderer->DrawShadowMap(); //绑定到深度帧缓冲或默认帧缓冲并clear，并渲染灯光空间的深度图

			//恢复viewport（这里除了可能被上面的渲染阴影贴图修改viewport，还可能被上一帧修改，比如后处理中的resizeViewport）
			renderer->ApplyViewportSize(renderer->ViewportWidth, renderer->ViewportHeight, false, false);

			//确定要写入的帧缓冲：若需要后处理，则写入到自定义帧缓冲
			if (hasPostProcessing)
			{
				renderer->SwitchAnotherCustomFrameBuffer(); //绑定并clear
			}
			//否则写入到默认帧缓冲
			else
			{
				renderer->SwitchDefaultFrameBuffer(); //绑定并clear
			}

			//判断是否启用多重采样
			bool useMultiSample = renderer->multiSample > 0 && (WindowMsaaSamples == 0 || hasPostProcessing);

			//若需要多重采样，则渲染到多重采样帧缓冲（虽然最终选择了多重采样帧缓冲，但上面的确定当前帧缓冲和clear依然要执行）
			if (useMultiSample)
			{ 
				if (displayShadowMap == false)
					renderer->BindMultiSampleFrameBuffer(); //绑定到多重采样帧缓冲并clear
				else
					renderer->stepCount++; //对齐上面BindMultiSampleFrameBuffer中clear占用的一次step
			}

			//应用光照数据（必须在渲染阴影贴图之后，因为DrawShadowMap中修改了光照的空间矩阵）
			renderer->ApplyLights();

			//渲染图像
			testMenu.OnRender();

			//若多重采样，则渲染结果位块传送到当前帧缓冲（自定义帧缓冲或默认帧缓冲）
			if (useMultiSample && displayShadowMap == false)
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

			// Capture actual rendered pixels before the optional inspector overlay.
			if (!CapturePath.empty() && frameCount == CaptureFrame && !CaptureWithUI)
				SaveCapture();
			//渲染UI
			gui.BeginFrame();
			testMenu.OnImGuiRender();
			//gui.ShowDemoWindow();
			gui.EndFrame();
			if (!CapturePath.empty() && frameCount == CaptureFrame && CaptureWithUI)
				SaveCapture();
			if (!CapturePath.empty() && frameCount >= CaptureFrame) QuitApp();

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

void Application::SaveCapture()
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	if (width <= 0 || height <= 0) throw std::runtime_error("Cannot capture an empty framebuffer.");
	std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * 3);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
	stbi_flip_vertically_on_write(true);
	auto captureDirectory = std::filesystem::path(std::u8string(CapturePath.begin(), CapturePath.end())).parent_path();
	if (!captureDirectory.empty()) std::filesystem::create_directories(captureDirectory);
	if (!stbi_write_png(CapturePath.c_str(), width, height, 3, pixels.data(), width * 3))
		throw std::runtime_error("Could not write framebuffer capture: " + CapturePath);
	std::cout << "Saved framebuffer: " << CapturePath << " (" << width << "x" << height << ")" << std::endl;
}

static int MainWithUtf8Arguments(int argc, char** argv)
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
	try {
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if (arg == "--help") {
			std::cout << "LearnOpengl [--preset cube|cube-flat|cube-normal|character|car|sphere|normals|wireframe|grayscale] "
				"[--capture output.png] [--hidden] [--with-ui] [--frames 12]" << std::endl;
			return 0;
		}
		else if (arg == "--hidden") app.HiddenWindow = true;
		else if (arg == "--with-ui") app.CaptureWithUI = true;
		else if (arg == "--capture" && i + 1 < argc) app.CapturePath = argv[++i];
		else if (arg == "--preset" && i + 1 < argc) app.ShowcasePreset = argv[++i];
		else if (arg == "--frames" && i + 1 < argc) {
			const std::string value = argv[++i];
			std::size_t consumed = 0;
			app.CaptureFrame = std::stoi(value, &consumed);
			if (consumed != value.size() || app.CaptureFrame < 1)
				throw std::invalid_argument("--frames requires a positive integer.");
		}
		else { std::cerr << "Unknown or incomplete argument: " << arg << std::endl; return 2; }
	}
	} catch (const std::exception& error) {
		std::cerr << "Invalid capture argument: " << error.what() << std::endl;
		return 2;
	}
	if (app.HiddenWindow && app.CapturePath.empty()) { std::cerr << "--hidden requires --capture." << std::endl; return 2; }
	try { return app.Run(); }
	catch (const std::exception& error) { std::cerr << error.what() << std::endl; return 1; }
}

#ifdef _WIN32
int wmain(int argc, wchar_t** wideArguments)
{
	// The CRT's narrow argv uses the system ANSI code page. Convert the original
	// UTF-16 Windows arguments explicitly before passing paths to stb/filesystem.
	std::vector<std::string> utf8Arguments;
	utf8Arguments.reserve(argc);
	for (int i = 0; i < argc; ++i) {
		const int size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wideArguments[i], -1, nullptr, 0, nullptr, nullptr);
		if (size <= 0) { std::cerr << "Invalid Windows command-line encoding." << std::endl; return 2; }
		std::string value(size, '\0');
		WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wideArguments[i], -1, value.data(), size, nullptr, nullptr);
		value.pop_back();
		utf8Arguments.push_back(std::move(value));
	}
	std::vector<char*> arguments;
	for (auto& value : utf8Arguments) arguments.push_back(value.data());
	return MainWithUtf8Arguments(argc, arguments.data());
}
#else
int main(int argc, char** argv) { return MainWithUtf8Arguments(argc, argv); }
#endif
