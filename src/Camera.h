#pragma once
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "IOnInspectorGUI.h"
#include <algorithm>
#include "Event.h"

struct Camera : IOnInspectorGUI {
	alignas(16) glm::mat4 ViewMatrix = glm::mat4(1.0f);
	alignas(16) glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
	alignas(4) float near = 1.0f; //近平面不能太小，否则会引起精度问题导致画面抖动，unity中最小为0.01
	alignas(4) float far = 10000;

	glm::mat4 ProjectionMatrix_ortho = glm::mat4(1.0f);
	glm::mat4 ProjectionMatrix_perspective = glm::mat4(1.0f);

	glm::vec3 position = glm::vec3(0, 0, 500);
	glm::vec3 direction = glm::vec3(0, 0, -1);
	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 GetRight() { return glm::normalize(glm::cross(direction, up)); }

	float fovy = 60;
	float aspect = 1;

	bool orthoGraphic;
	float orthoRatio;
	glm::vec4 orthoRect;


	Event<> ProjectionMatrixChangedEvent;
	Event<> ViewMatrixChangedEvent;

	bool orthoGraphicIndividualSetting;
	glm::mat4 lerpMat4(glm::mat4& a, glm::mat4& b, float t) {
		return a + (b - a) * t;
	}
	void UpdateProjectionMatrix()
	{
		//std::cout << "update fov y : " << fovy << std::endl;
		if (orthoRect.w - orthoRect.z > 0)
			aspect = (orthoRect.y - orthoRect.x) / (orthoRect.w - orthoRect.z); //TODO：这句应该放在orthoRect的属性set里
		ProjectionMatrix_ortho = glm::ortho(orthoRect.x, orthoRect.y, orthoRect.z, orthoRect.w, near, far);
		ProjectionMatrix_perspective = glm::perspective(glm::radians(fovy), aspect, near, far);
		float t = (1 - 1 / orthoRatio) * 0.002 + 1; //这个参数t的值是经测试取到变化最大的范围
		if (t < 0) t = 0;
		ProjectionMatrix = lerpMat4(ProjectionMatrix_perspective, ProjectionMatrix_ortho, t);
		ProjectionMatrixChangedEvent.Invoke();
	}

	void UpdateViewMatrix()
	{
		ViewMatrix = glm::lookAt(position, position + direction, up);
		ViewMatrixChangedEvent.Invoke();
	}

	void UpdateOrthoRectByViewport(int viewportWidth, int viewportHeight) {
		orthoRect = glm::vec4(-viewportWidth / 4.0f, viewportWidth / 4.0f, -viewportHeight / 4.0f, viewportHeight / 4.0f);
	}

	// 通过 IOnInspectorGUI 继承
	void OnInspectorGUI() override;
};
