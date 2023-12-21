#pragma once

#include <string>
#include "PostProcessingMaterial.h"
#include "Texture.h"
#include "imgui.h"
#include "Application.h"

class ResizeViewportMaterial : public PostProcessingMaterial
{
public:
	bool followWindowSize = true;
	mutable glm::ivec2 viewportSize;
	std::string GetName() const override { return "Resize Viewport"; }
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/PostProcess/DoNothing.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		PostProcessingMaterial::ApplyUniforms(modelMatrix);
		if (followWindowSize)
		{
			FollowWindowSize();
		}
		Application::GetInstance()->renderer->ApplyViewportSize(viewportSize.x, viewportSize.y, false, false);
		Application::GetInstance()->renderer->NeedReapplyCurrentViewportSize = true; //需要在下一帧开始时恢复原来的viewportSize
	}

	void FollowWindowSize() const {
		auto renderer = Application::GetInstance()->renderer.get();
		viewportSize = glm::ivec2(renderer->WindowWidth, renderer->WindowHeight);
	}

	ResizeViewportMaterial()
	{
		FollowWindowSize();
	}

	void DrawImgui(int id) override
	{
		std::string idText = "##" + std::to_string(id);
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(100);
		if (ImGui::DragInt2(("Viewport Size" + idText).c_str(), &viewportSize.x, 1, 1, 99999));
		ImGui::SameLine();
		ImGui::Checkbox(("Follow Window Size" + idText).c_str(), &followWindowSize);

	}

	ResizeViewportMaterial* CreateObject() override {
		return new ResizeViewportMaterial();
	}
};

