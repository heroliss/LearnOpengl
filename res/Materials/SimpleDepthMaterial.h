#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class SimpleDepthMaterial : public Material
{
public:
	glm::mat4 lightSpaceMatrix;
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/simpleDepthShader.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		shader->SetUniformMat4f("u_Model", modelMatrix);
		shader->SetUniformMat4f("lightSpaceMatrix", lightSpaceMatrix);
	}
};

