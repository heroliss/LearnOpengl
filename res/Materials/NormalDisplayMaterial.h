#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class NormalDisplayMaterial : public Material
{
public:
	float size = 10;
	bool showVertexNormal;
	bool showFragmentNormal;
	float showFragmentNormalInterval = 0; //该值为0或负数时，表示启用下面的showFragmentNormalSamplesSize
	glm::vec2 showFragmentNormalSamplesSize = glm::vec2(10, 10);
	std::shared_ptr<Texture> NormalTexture = Texture::Get(128, 128, 255);

	std::string GetShaderFilePath() const override
	{
		return "res/shaders/normalDisplayShader.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		shader->SetUniformMat4f("u_Model", modelMatrix);

		shader->SetUniform1f("size", size);
		shader->SetUniform1i("showVertexNormal", showVertexNormal);
		shader->SetUniform1i("showFragmentNormal", showFragmentNormal);
		shader->SetUniform1f("showFragmentNormalInterval", showFragmentNormalInterval);
		shader->SetUniform2f("showFragmentNormalSamplesSize", showFragmentNormalSamplesSize.x, showFragmentNormalSamplesSize.y);
		shader->SetUniform1i("u_normalTexture", 0);
		if (NormalTexture != nullptr) NormalTexture->SetUnit(0); else NormalTexture->UnsetUnit(0);
	}
};

