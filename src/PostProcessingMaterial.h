#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class PostProcessingMaterial : public Material
{
public:
	virtual std::string GetName() const = 0;
	virtual void DrawImgui(int id) = 0;

	std::vector<std::shared_ptr<Texture>> RenderTargets;
	std::shared_ptr<Texture> ErrorTexture = Texture::Get("ErrorTexture");

	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		for (int i = 0; i < RenderTargets.size(); i++)
		{
			shader->SetUniform1i(std::format("u_screenTextures[{}]", i), i);
			RenderTargets[i]->SetUnit(i);
		}
	}

	virtual PostProcessingMaterial* CreateObject() = 0;
};

