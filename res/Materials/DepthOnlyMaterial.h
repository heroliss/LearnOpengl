#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class DepthOnlyMaterial : public Material
{
public:
	int cubemapDebugShowLayer;
	bool useCubemap;
	glm::vec3 lightPos;
	glm::mat4 lightSpaceMatrix[6]; //点光源需要用到6个方向的矩阵，其他光源只需用到0位置矩阵
	glm::vec2 nearAndFar;
	glm::vec2 showDepthRange;
	bool linearizeDepth;

	std::string GetShaderFilePath() const override
	{
		return useCubemap ? "res/shaders/depthOnlyCubemap.glsl" : "res/shaders/depthOnly.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		shader->SetUniformMat4f("u_Model", modelMatrix);
		shader->SetUniform2f("u_nearAndFar", nearAndFar.x, nearAndFar.y);
		shader->SetUniform2f("u_showDepthRange", showDepthRange.x, showDepthRange.y);
		shader->SetUniform1i("u_LinearizeDepth", linearizeDepth);
		shader->SetUniformMat4fv("lightSpaceMatrix", lightSpaceMatrix, useCubemap ? 6 : 1);
		if (useCubemap) {
			shader->SetUniform3f("u_lightPos", lightPos.x, lightPos.y, lightPos.z);
			shader->SetUniform1i("u_debugShowLayer", cubemapDebugShowLayer);
		}
	}
};

