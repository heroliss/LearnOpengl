#pragma once

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

class GUI
{
public:
	GUI(const char* glsl_version, GLFWwindow* window);
	~GUI();
	void BeginFrame();
	void EndFrame();
	void ShowDemoWindow();
private:
	ImGuiIO* io;
	GLFWwindow* window;
	const char* glsl_version;
};

