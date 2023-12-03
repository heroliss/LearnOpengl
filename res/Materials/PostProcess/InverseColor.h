#pragma once

#include "PostProcessingMaterial.h"
#include <Texture.h>
#include <string>

class InverseColor : public PostProcessingMaterial
{
public:
	std::string GetName() const override { return "Inverse color"; }
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/PostProcess/InverseColor.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		PostProcessingMaterial::ApplyUniforms(modelMatrix);
	}

	void DrawImgui(int id) override
	{

	}

	InverseColor* CreateObject() override {
		return new InverseColor();
	}
};

