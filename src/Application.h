#pragma once

#include "Renderer.h"
#include "Input.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <memory>


class Application
{
private:
	static Application* m_instance;

	GLFWwindow* window;
	glm::vec3 translation = glm::vec3(0, 0, 0);

	//TODO: 这个方法不知道怎么写成非静态的，去掉static会报错
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

public:
	static Application* GetInstance() { return m_instance; }
	Application();
	~Application();

	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<Input> input;
 	const GLFWvidmode* videoMode;

	int Run();

	bool needRestart = false;
	int WindowMsaaSamples = 0; //窗口全局的多重采样设置

	float Time = 0.0;
	float DeltaTime = 0.0;
	float TargetFrameRate = 10000; // 目标帧率
	int frameCount = 0;

	bool AutoResizeViewportByWindow = true;

	void ResetWindow(GLFWmonitor* monitor = nullptr, int refreshRate = GLFW_DONT_CARE, int width = 0, int height = 0) {
		bool fullScreen = monitor != nullptr;
		if (width == 0) width = fullScreen ? videoMode->width : videoMode->width * 0.7;
		if (height == 0) height = fullScreen ? videoMode->height : videoMode->height * 0.7;
		glfwSetWindowMonitor(window, monitor, 0, 0, width, height, refreshRate);
		renderer->WindowWidth = width;
		renderer->WindowHeight = height;
		if (!fullScreen) {
			glfwSetWindowPos(window, videoMode->width * 0.15, videoMode->height * 0.15); //往里移动一点，以防看不见窗口的标题栏
		}
	}

	void QuitApp() {
		glfwSetWindowShouldClose(window, true); // 设置标志以通知应用程序关闭窗口
	}
};