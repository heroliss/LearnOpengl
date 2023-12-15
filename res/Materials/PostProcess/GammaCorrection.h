#pragma once

#include <string>
#include "PostProcessingMaterial.h"
#include "Texture.h"
#include "imgui.h"
#include "Application.h"

class GammaCorrection : public PostProcessingMaterial
{
public:
	float gamma = 2.2f;
	bool followSRGB = true;
	std::string GetName() const override { return "Gamma Correction"; }
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/PostProcess/GammaCorrection.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		PostProcessingMaterial::ApplyUniforms(modelMatrix);
		Shader& shader = *GetShader();
		shader.SetUniform1f("gamma", gamma);
	}

	void DrawImgui(int id) override
	{
		std::string idText = "##" + std::to_string(id);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat(("gamma" + idText).c_str(), &gamma, 0.01);
		ImGui::SameLine();
		ImGui::Checkbox(("follow sRGB" + idText).c_str(), &followSRGB);
		if (followSRGB) {
			gamma = Texture::enableSRGB ? 2.2f : 1.0f;
		}
	}

	GammaCorrection* CreateObject() override {
		return new GammaCorrection();
	}
};

