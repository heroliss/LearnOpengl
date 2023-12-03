#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class PostProcessingMaterial : public Material
{
public:
	virtual std::string GetName() const = 0;
	virtual void DrawImgui(int id) = 0;

	std::shared_ptr<Texture> ScreenTexture;
	std::shared_ptr<Texture> ErrorTexture = Texture::Get("ErrorTexture");

	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		shader->SetUniform1i("screenTexture", 0);
		if (ScreenTexture) ScreenTexture->SetUnit(0); else ErrorTexture->SetUnit(0);
	}

	virtual PostProcessingMaterial* CreateObject() = 0;
};

