#pragma once
#include "glm/glm.hpp"
#include "Shader.h"
#include "glm/gtx/euler_angles.hpp"
#include "Event.h"

#define MAX_LIGHT_COUNT 12

enum LightType : int32_t {
	NONE_LIGHT = 0,
	PARALLEL_LIGHT = 1,
	POINT_LIGHT = 2,
	SPOT_LIGHT = 3
};

struct Light
{
public:
	alignas(4)  LightType type          = LightType::PARALLEL_LIGHT;
	alignas(4)  bool       useBlinnPhong = 1;
	alignas(4)  float     brightness = 1;                                       //亮度，与颜色相乘
	alignas(16) glm::vec3 pos           = glm::vec3(-100, 100, 100);            //光源位置 (平行光无用)
	alignas(16) glm::vec3 direction     = glm::normalize(glm::vec3(1, -1, -1)); //照射方向（点光源无用）
	alignas(16) glm::vec3 color         = glm::vec3(1);
	alignas(16) glm::vec3 attenuation   = glm::vec3(1.0, 0.0001, 0.00002);		//衰减系数 （分别为常数项、一次项、二次项系数。一般常数项固定为1，主要调二次项系数）
	alignas(16) glm::vec2 cutoffAngle   = glm::vec2(0, 25);					    //聚光范围 (内圈和外圈，度数表示，仅聚光类型有用)

	void* GetData() {
		return &type;
	}

	static void RemoveLight(Shader* shader, int index) {
		std::string s = "u_lights[" + std::to_string(index) + "].";
		shader->SetUniform1i(s + "type", LightType::NONE_LIGHT);
	}

	static unsigned long long GetSize() {
		return offsetof(Light, cutoffAngle) + 16;
	}



	//其他展示功能
	bool autoRotate = false;
	float autoRotateSpeed; //每秒旋转角度
	glm::vec3 autoRotateAxis = glm::vec3(0.0f, 0.0f, -1.0f); //自动旋转轴
	glm::vec3 autoRotateAxisEulerAngles = glm::vec3(0); //自动旋转轴的欧拉角表示
	bool autoLookAtCenter = false; //自动对准中心点
	bool showGizmo = true;
	float gizmoSize = 5;

	void UpdateAutoRotateAxisByEulerAngles() {
		glm::mat4 rotationMatrix = glm::eulerAngleXYZ(autoRotateAxisEulerAngles.x, autoRotateAxisEulerAngles.y, autoRotateAxisEulerAngles.z);
		autoRotateAxis = rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
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
