#pragma once

#include "GLFW/glfw3.h"

class Input {
public:
	Input(GLFWwindow* window);
	void processInput(GLFWwindow* window);
	glm::mat4 ModelMatrix = glm::mat4(1.0f);

private:
	void mouseButtonCallback(int button, int action, int mods);
	void cursorPosCallback(double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	GLFWwindow* window;
	double lastX;
	double lastY;
	bool pressed;
	bool pressedRight;

	float cameraRotateSpeed = 0.0015f;
	float cameraMaxSpeed = 300.0f;
	float cameraMaxHighSpeed = 1000.0f;
	float modelRotateSpeed = 0.01f;
	float modelMaxSpeed = 20.0f;
};
