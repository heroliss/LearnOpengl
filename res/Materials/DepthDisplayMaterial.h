#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class DepthDisplayMaterial : public Material
{
public:
	glm::vec2 range = glm::vec2(1, 1000);
	float orthoRatio;
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/depthDisplay.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		shader->SetUniformMat4f("u_Model", modelMatrix);
		shader->SetUniform2f("u_showDepthRange", range.x, range.y);
		shader->SetUniform1f("u_orthoRatio", orthoRatio);
	}
};

