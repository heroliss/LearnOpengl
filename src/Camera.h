#pragma once
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "IOnInspectorGUI.h"

struct Camera : IOnInspectorGUI {
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
	bool orthoGraphic;
	glm::vec4 orthoRect;

	bool orthoGraphicIndividualSetting;

	void UpdateProjectionMatrix()
	{
		//std::cout << "update fov y : " << fovy << std::endl;
		if (orthoGraphic)
		{
			if (orthoRect.w - orthoRect.z > 0)
				aspect = (orthoRect.y - orthoRect.x) / (orthoRect.w - orthoRect.z); //TODO：这句应该放在orthoRect的属性set里
			ProjectionMatrix = glm::ortho(orthoRect.x, orthoRect.y, orthoRect.z, orthoRect.w, near, far);
		}
		else
		{
			ProjectionMatrix = glm::perspective(glm::radians(fovy), aspect, near, far);
		}
	}

	void UpdateViewMatrix()
	{
		ViewMatrix = glm::lookAt(position, position + direction, up);
	}

	void UpdateOrthoRectByViewport(int viewportWidth, int viewportHeight) {
		orthoRect = glm::vec4(-viewportWidth / 4.0f, viewportWidth / 4.0f, -viewportHeight / 4.0f, viewportHeight / 4.0f);
	}

	// 通过 IOnInspectorGUI 继承
	void OnInspectorGUI() override;
};
