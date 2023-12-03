#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>

class BaseMaterial_Blue : public Material
{
public:
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Texture> texture2;

	~BaseMaterial_Blue() {
		std::cout << "销毁BaseMaterial_Blue！！！！！！！！！！！" << std::endl;
	}

	BaseMaterial_Blue() {
		texture = Texture::Get("res/textures/111.jpg", true);
		texture2 = Texture::Get("res/textures/222.jpg", true);
		//设置参数：
	/*	texture.setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
		texture.setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
		texture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		texture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

	}

	std::string GetShaderFilePath() const override
	{
		return "res/shaders/basic.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		ApplyMVPUniforms(modelMatrix);

		auto shader = GetShader();
		shader->SetUniform4f("u_Color", 0.9f, 0.8f, 1, 1);
		shader->SetUniform1f("u_TrapezoidFactor", 0.3f);

		texture->SetUnit(0);
		texture2->SetUnit(1);

		int samplers[2] = { 0,1 };
		shader->SetUniform1iv("u_Textures", 2, samplers);
	}
};

