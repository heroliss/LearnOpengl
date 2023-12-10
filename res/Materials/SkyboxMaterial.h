#pragma once

#include "Material.h"
#include <Texture.h>
#include <string>
#include "Cubemap.h"

class SkyboxMaterial : public Material
{
public:
	std::shared_ptr<Cubemap> cubemap;
	std::string GetShaderFilePath() const override
	{
		return "res/shaders/Skybox.glsl";
	}
	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		auto shader = GetShader();
		auto camera = Application::GetInstance()->renderer->camera;

		shader->SetUniformMat4f("u_View", glm::mat4(glm::mat3(camera.ViewMatrix))); //只保留视图矩阵的旋转部分
		shader->SetUniformMat4f("u_Projection", camera.ProjectionMatrix_perspective); //无论如何使用透视投影矩阵


		shader->SetUniform1i("cubemap", 0);
		if (cubemap != nullptr) cubemap->SetUnit(0); else Cubemap::UnsetUnit(0);
	}
};

