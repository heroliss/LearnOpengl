#pragma once
#include "glm/glm.hpp"
#include "Shader.h"
#include "glm/gtx/euler_angles.hpp"
#include "Event.h"
#include "Texture.h"
#include "Cubemap.h"

#define MAX_LIGHT_COUNT 4

enum LightType : int32_t {
	NONE_LIGHT = 0,
	PARALLEL_LIGHT = 1,
	POINT_LIGHT = 2,
	SPOT_LIGHT = 3
};

struct Light
{
public:
	alignas(4)  LightType type = LightType::PARALLEL_LIGHT;
	alignas(4)  int       useBlinnPhong = 1;
	alignas(4)  float     brightness = 0.5f;                                    //亮度，与颜色相乘
	alignas(4)  int       castShadow = 1;
	alignas(4)  int       shadowPCFSize = 1;                                    //PCF(Percentage Closer Filtering) 采样尺寸的边长
	alignas(4)  float     shadowBias;                                           //光照与法线接近垂直时的最大阴影深度偏移值（跟随shadowBiasSettingValue和shadowNearAndFar变化）
	alignas(4)  float     shadowBiasChangeRate = 70;                            //光照与法线由平行到垂直变化时引起的阴影深度偏移值变化的速率
	alignas(4)  float     shadowSampleDiskRadius = 0.1f;
	alignas(16) glm::vec3 pos = glm::vec3(-100, 100, 100);            //光源位置 (平行光无用)
	alignas(16) glm::vec3 direction = glm::normalize(glm::vec3(1, -1, -1)); //照射方向（点光源无用）
	alignas(16) glm::vec3 color = glm::vec3(1);
	alignas(16) glm::vec3 attenuation = glm::vec3(1.0, 0.0001, 0.00001);		//衰减系数 （分别为常数项、一次项、二次项系数。一般常数项固定为1，主要调二次项系数）
	alignas(8) glm::vec2  cutoffAngle = glm::vec2(0, 25);					    //聚光范围 (内圈和外圈，度数表示，仅聚光类型有用)
	alignas(8) glm::vec2  shadowNearAndFar = glm::vec2(1.0f, 10000.0f);         //阴影投影体的近远平面
	alignas(16) glm::mat4 lightSpaceMatrix;                                     //光照空间矩阵（只有渲染阴影贴图时才会被填充，只有非点光源会用到）

	std::shared_ptr<Texture> shadowTexture; //用于平行光和聚光灯
	std::shared_ptr<Cubemap> shadowCubemap; //用于点光源

	void* GetData() {
		return &type;
	}

	static unsigned long long GetSize() {
		return offsetof(Light, shadowTexture);
	}

	Light() {
		UpdateShadowBiasBySettingValue();
	}


	//其他展示功能
	bool autoRotate = false;
	float autoRotateSpeed = 0; //每秒旋转角度
	const glm::vec3 DefualtAutoRotateAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 autoRotateAxis = DefualtAutoRotateAxis; //自动旋转轴
	glm::vec3 autoRotateAxisEulerAngles = glm::vec3(0); //自动旋转轴的欧拉角表示
	bool autoLookAtCenter = true; //自动对准中心点
	bool showGizmo = true;
	float gizmoSize = 5;

	//阴影设置	
	glm::uvec2 shadowTextureSize = glm::uvec2(4096, 4096);
	glm::vec4 shadowOrthoRect = glm::vec4(-500.0f, 500.0f, -500.0f, 500.0f); //用于平行光的正交矩阵
	glm::vec2 shadowShowDepthRange = glm::vec2(1.0f, 1000.0f); //仅用于展示深度图
	bool shadowFrontFaceCulling = false;
	float shadowBiasSettingValue = 0.03f;
	void UpdateShadowBiasBySettingValue() {
		shadowBias = shadowBiasSettingValue;
		switch (type)
		{
		case NONE_LIGHT:
			break;
		case PARALLEL_LIGHT:
			shadowBias = 800 * shadowBiasSettingValue / (shadowNearAndFar.y - shadowNearAndFar.x); //TODO：为什么平行光的bias需要根据远近平面变化？
			break;
		case POINT_LIGHT:
			shadowBias = 1000 * shadowBiasSettingValue; //这里的倍数也是随便加的，用于在相同bias时和其他光源的效果大致相当
			break;
		case SPOT_LIGHT:
			break;
		default:
			break;
		}
	}

	void UpdateAutoRotateAxisByEulerAngles() {
		glm::mat4 rotationMatrix = glm::eulerAngleXYZ(autoRotateAxisEulerAngles.x, autoRotateAxisEulerAngles.y, autoRotateAxisEulerAngles.z);
		autoRotateAxis = rotationMatrix * glm::vec4(DefualtAutoRotateAxis, 0);
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
