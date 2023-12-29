#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class DepthOnlyMaterial : public Material
{
public:
	glm::mat4 lightSpaceMatrix;
	glm::vec2 nearAndFar;
	glm::vec2 showDepthRange;
	bool linearizeDepth;

	std::string GetShaderFilePath() const override
	{
		return "res/shaders/depthOnly.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		shader->SetUniformMat4f("u_Model", modelMatrix);
		shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
		shader->SetUniform2f("u_nearAndFar", nearAndFar.x, nearAndFar.y);
		shader->SetUniform2f("u_showDepthRange", showDepthRange.x, showDepthRange.y);
		shader->SetUniform1i("u_LinearizeDepth", linearizeDepth);
	}
};

