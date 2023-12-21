#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class SingleColorMaterial : public Material
{
public:
	glm::vec4 color = glm::vec4(0, 1, 1, 1);
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/singleColor.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		shader->SetUniformMat4f("u_Model", modelMatrix);

		shader->SetUniform4f("u_singleColor", color.r, color.g, color.b, color.a);
	}
};

