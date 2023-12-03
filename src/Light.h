#pragma once
#include "glm/glm.hpp"
#include "Shader.h"
#include "glm/gtx/euler_angles.hpp"

#define MAX_LIGHT_COUNT 8

enum LightType {
	NONE_LIGHT = 0,
	PARALLEL_LIGHT = 1,
	POINT_LIGHT = 2,
	SPOT_LIGHT = 3
};

class Light
{
public:
	LightType type = LightType::POINT_LIGHT;
	glm::vec3 pos = glm::vec3(-100, 100, 100);                        //光源位置 (平行光无用)
	glm::vec3 direction = glm::normalize(glm::vec3(1, -1, -1));   //照射方向（点光源无用）
	glm::vec3 color = glm::vec3(1);
	glm::vec3 attenuation = glm::vec3(1.0, 0.0001, 0.00002);		 //衰减系数 （分别为常数项、一次项、二次项系数。一般常数项固定为1，主要调二次项系数）
	glm::vec2 cutoffAngle = glm::vec2(0, 25);					 //聚光范围 (内圈和外圈，度数表示，仅聚光类型有用)

	//其他展示功能
	float brightness = 1; //亮度，与颜色相乘
	bool autoRotate = false;
	float autoRotateSpeed; //每秒旋转角度
	glm::vec3 autoRotateAxis = glm::vec3(0.0f, 0.0f, -1.0f); //自动旋转轴
	glm::vec3 autoRotateAxisEulerAngles = glm::vec3(0); //自动旋转轴的欧拉角表示
	bool autoLookAtCenter = false; //自动对准中心点
	bool showGizmo = true;
	float gizmoSize = 5;
	bool useBlinnPhong = true;

	void UpdateAutoRotateAxisByEulerAngles() {
		glm::mat4 rotationMatrix = glm::eulerAngleXYZ(autoRotateAxisEulerAngles.x, autoRotateAxisEulerAngles.y, autoRotateAxisEulerAngles.z);
		autoRotateAxis = rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	}

	void SetToShader(Shader* shader, int index) {
		std::string s = "u_lights[" + std::to_string(index) + "].";
		shader->SetUniform1i(s + "type", type);
		shader->SetUniform3f(s + "pos", pos.x, pos.y, pos.z);
		shader->SetUniform3f(s + "direction", direction.x, direction.y, direction.z);
		shader->SetUniform3f(s + "color", color.x * brightness, color.y * brightness, color.z * brightness);
		shader->SetUniform3f(s + "attenuation", attenuation.x, attenuation.y, attenuation.z);
		shader->SetUniform2f(s + "cutoffAngle", cutoffAngle.x, cutoffAngle.y);
		shader->SetUniform1i(s + "useBlinnPhong", useBlinnPhong);
	}

	static void RemoveLight(Shader* shader, int index) {
		std::string s = "u_lights[" + std::to_string(index) + "].";
		shader->SetUniform1i(s + "type", LightType::NONE_LIGHT);
	}

	static std::string LightTypeToString(LightType lightType) {
		std::string s;
		switch (lightType)
		{
		case NONE_LIGHT:
			s = "no light";
			break;
		case PARALLEL_LIGHT:
			s = "parallel light";
			break;
		case POINT_LIGHT:
			s = "point light";
			break;
		case SPOT_LIGHT:
			s = "spot light";
			break;
		default:
			break;
		}
		return s;
	}
};
