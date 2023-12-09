#include <iostream>
#include "Application.h"
#include "Input.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/quaternion.hpp"
#include "imgui.h"

#define KEY_PRESS(key) \
  else if (glfwGetKey(window, key) == GLFW_PRESS)

Input::Input(GLFWwindow* window) : window(window), lastX(0.0f), lastY(0.0f), pressed(false), pressedRight(false) {
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
		input->mouseButtonCallback(button, action, mods);
		});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
		input->cursorPosCallback(xpos, ypos);
		});

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
		input->scroll_callback(window, xoffset, yoffset);
		});

	glfwSetWindowUserPointer(window, this);
}

void Input::mouseButtonCallback(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &lastX, &lastY);
		pressed = true;
		pressedRight = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &lastX, &lastY);
		pressed = false;
		pressedRight = true;
	}

	else {
		pressed = false;
		pressedRight = false;
	}
}

void Input::cursorPosCallback(double xpos, double ypos) {
	//使用Imgui操作时禁用鼠标操作
	if (ImGui::IsAnyItemActive())
	{
		return;
	}

	//左键按下
	if (pressed) {
		float xOffset = xpos - lastX;
		float yOffset = lastY - ypos;
		if (xOffset == 0 && yOffset == 0) return; //这句必须有，否则会出现意外的黑屏

		// 处理鼠标拖动事件
		auto renderer = Application::GetInstance()->renderer.get();

		// 计算鼠标偏移量的方向
		glm::vec2 mouseDelta = glm::vec2(xOffset, yOffset);
		glm::vec4 axis = glm::vec4(-mouseDelta.y, mouseDelta.x, 0, 0);
		axis = glm::inverse(renderer->camera.ViewMatrix) * axis;
		axis = glm::inverse(ModelMatrix) * axis;

		// 模型旋转
		ModelMatrix = glm::rotate(ModelMatrix, glm::length(mouseDelta) * modelRotateSpeed, glm::vec3(axis));

		//std::cout << "x:" << xOffset << "   y:" << yOffset << std::endl;

		lastX = xpos;
		lastY = ypos;
	}
	//右键按下
	else if (pressedRight) {
		float xOffset = xpos - lastX;
		float yOffset = lastY - ypos;
		if (xOffset == 0 && yOffset == 0) return; //这句必须有，否则会出现意外的黑屏

		// 处理鼠标拖动事件
		auto renderer = Application::GetInstance()->renderer.get();
		// 更新摄像机方向
		glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), -xOffset * cameraRotateSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), yOffset * cameraRotateSpeed, renderer->camera.GetRight());
		renderer->camera.direction = rotationMatrixY * rotationMatrixX * glm::vec4(renderer->camera.direction, 0);
		//renderer->camera.up = rotationMatrixY * rotationMatrixX * glm::vec4(renderer->camera.up, 0);

		//glm::extractEulerAngleXYZ
		//glm::quat rotationQuat = glm::toQuat(rotationMatrix);

		renderer->camera.UpdateViewMatrix();

		//std::cout << "x:" << xOffset << "   y:" << yOffset << std::endl;
		//std::cout << "xDirection" <<std::endl << glm::to_string(xDirection) << std::endl;

		lastX = xpos;
		lastY = ypos;
	}
}

void Input::processInput(GLFWwindow* window) // 处理键盘输入
{
	auto app = Application::GetInstance();
	//使用Imgui聚焦时禁用键盘操作
	if (ImGui::IsAnyItemFocused())
	{
		return;
	}

	float cameraSpeed = cameraMaxSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
	{
		cameraSpeed = cameraMaxHighSpeed;
	}

	auto renderer = Application::GetInstance()->renderer.get();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // 按下ESC键，关闭窗口
	{
		app->QuitApp();
	}
	//模型移动
	KEY_PRESS(GLFW_KEY_LEFT)
	{
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-modelMaxSpeed * app->DeltaTime, 0, 0));
	}
	KEY_PRESS(GLFW_KEY_RIGHT)
	{
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(modelMaxSpeed * app->DeltaTime, 0, 0));
	}
	KEY_PRESS(GLFW_KEY_UP)
	{
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, 0, -modelMaxSpeed * app->DeltaTime));
	}
	KEY_PRESS(GLFW_KEY_DOWN)
	{
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0, 0, modelMaxSpeed * app->DeltaTime));
	}
	//视角移动
	KEY_PRESS(GLFW_KEY_W)
	{
		renderer->camera.position += renderer->camera.direction * cameraSpeed * app->DeltaTime;
		renderer->camera.UpdateViewMatrix();
	}
	KEY_PRESS(GLFW_KEY_S)
	{
		renderer->camera.position -= renderer->camera.direction * cameraSpeed * app->DeltaTime;
		renderer->camera.UpdateViewMatrix();
	}
	KEY_PRESS(GLFW_KEY_A)
	{
		renderer->camera.position -= renderer->camera.GetRight() * cameraSpeed * app->DeltaTime;
		renderer->camera.UpdateViewMatrix();
	}
	KEY_PRESS(GLFW_KEY_D)
	{
		renderer->camera.position += renderer->camera.GetRight() * cameraSpeed * app->DeltaTime;
		renderer->camera.UpdateViewMatrix();
	}
	KEY_PRESS(GLFW_KEY_Q)
	{
		renderer->camera.position += glm::vec3(0, -1, 0) * cameraSpeed * app->DeltaTime;
		renderer->camera.UpdateViewMatrix();
	}
	KEY_PRESS(GLFW_KEY_E)
	{
		renderer->camera.position += glm::vec3(0, 1, 0) * cameraSpeed * app->DeltaTime;
		renderer->camera.UpdateViewMatrix();
	}
}

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//鼠标在Imgui上时禁用滚轮操作
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		return;
	}
	//上下滚轮只有yoffset，从-3到3，xoffset恒为0
	//std::cout << "scroll_callback " << xoffset << "  " << yoffset << std::endl;
	auto renderer = Application::GetInstance()->renderer.get();

	if (renderer->camera.orthoGraphic == false)
	{
		if (renderer->camera.fovy >= 1.0f && renderer->camera.fovy <= 150.0f)
			renderer->camera.fovy -= yoffset * 5;
		if (renderer->camera.fovy <= 1.0f)
			renderer->camera.fovy = 1.0f;
		if (renderer->camera.fovy >= 150.0f)
			renderer->camera.fovy = 150.0f;
	}
	else
	{
		renderer->camera.orthoRect *= 1 - yoffset * 0.1;
	}

	renderer->camera.UpdateProjectionMatrix();
}
