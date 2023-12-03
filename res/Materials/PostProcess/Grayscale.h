#pragma once

#include <string>
#include "PostProcessingMaterial.h"
#include "Texture.h"
#include "imgui.h"

class Grayscale : public PostProcessingMaterial
{
public:
	std::string GetName() const override { return "Grayscale"; }
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/PostProcess/Grayscale.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		PostProcessingMaterial::ApplyUniforms(modelMatrix);
		Shader& shader = *GetShader();
		shader.SetUniform1f("GrayscalePercentage", GrayscalePercentage);
	}

	void DrawImgui(int id) override
	{
		std::string idText = "##" + std::to_string(id);
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(50);
		ImGui::DragFloat(("Grayscale Percentage" + idText).c_str(), &GrayscalePercentage, 0.01f, 0, 1);
	}

	Grayscale* CreateObject() override {
		return new Grayscale();
	}

	float GrayscalePercentage = 1;
};

