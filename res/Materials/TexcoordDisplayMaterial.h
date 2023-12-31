﻿#pragma once

#include "Material.h"
#include <string>

class TexcoordDisplayMaterial : public Material
{
public:
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/TexcoordDisplayShader.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		shader->SetUniformMat4f("u_Model", modelMatrix);

	}
};

