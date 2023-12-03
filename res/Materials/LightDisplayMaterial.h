#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class LightDisplayMaterial : public Material
{
public:
	int lightIndex;
	float gizmoSize;

	std::string GetShaderFilePath() const override
	{
		return "res/shaders/lightDisplayShader.glsl";
	}

	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		ApplyMVPUniforms(modelMatrix);

		auto shader = GetShader();
		ApplyAllLightUniforms();
		shader->SetUniform1i("u_lightIndex", lightIndex);
		shader->SetUniform1f("u_size", gizmoSize);
	}
};

