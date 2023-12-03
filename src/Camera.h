#pragma once
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

struct Camera {
	glm::mat4 ViewMatrix = glm::mat4(1.0f);
	glm::mat4 ProjectionMatrix = glm::mat4(1.0f);

	glm::vec3 position = glm::vec3(0, 0, 500);
	glm::vec3 direction = glm::vec3(0, 0, -1);
	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 GetRight() { return glm::normalize(glm::cross(direction, up)); }
	float fovy = 60;
	float aspect = 1;
	float near = 1.0f; //近平面不能太小，否则会引起精度问题导致画面抖动，unity中最小为0.01
	float far = 10000;

	void UpdateProjectionMatrix()
	{
		//std::cout << "update fov y : " << fovy << std::endl;
		ProjectionMatrix = glm::perspective(glm::radians(fovy), aspect, near, far);
		//这里是特殊情况，即屏幕尺寸的正交投影矩阵，可根据需求改变
		//ProjectionMatrix = glm::ortho(-ScreenWidth / 2.0f, ScreenWidth / 2.0f, -ScreenHeight / 2.0f, ScreenHeight / 2.0f, 0.0f, 100.0f);
	}

	void UpdateViewMatrix()
	{
		ViewMatrix = glm::lookAt(position, position + direction, up);
	}
};
