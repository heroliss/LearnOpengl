#pragma once

#include <string>
#include "PostProcessingMaterial.h"
#include "Texture.h"
#include "imgui.h"
#include "Application.h"

class ToneMapping : public PostProcessingMaterial
{
public:
	float gamma = 2.2f;
	bool followSRGB = true;
	bool useACESToneMapping = true;
	float adapted_lum = 1;
	std::string GetName() const override { return "Tone Mapping"; }
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/PostProcess/ToneMapping.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		PostProcessingMaterial::ApplyUniforms(modelMatrix);
		Shader& shader = *GetShader();
		shader.SetUniform1f("gamma", gamma);
		shader.SetUniform1i("useACESToneMapping", useACESToneMapping);
		shader.SetUniform1f("adapted_lum", adapted_lum);
	}

	void DrawImgui(int id) override
	{
		std::string idText = "##" + std::to_string(id);

		ImGui::SameLine(120);

		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat(("gamma" + idText).c_str(), &gamma, 0.01);
		ImGui::SameLine();
		ImGui::Checkbox(("follow sRGB" + idText).c_str(), &followSRGB);
		if (followSRGB) {
			gamma = Texture::enableSRGB ? 2.2f : 1.0f;
		}

		ImGui::SetCursorPosX(120);

		ImGui::Checkbox("ACES ToneMapping", &useACESToneMapping);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::DragFloat("adapted_lum", &adapted_lum, 0.001f, 0, 999);
	}

	ToneMapping* CreateObject() override {
		return new ToneMapping();
	}
};

