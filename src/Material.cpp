#include "Material.h"

#include <memory>
#include <iostream>
#include "Application.h"
#include "Light.h"


std::unordered_map<std::string, Shader*> Material::m_ShaderCache;

Shader* Material::GetShader() const
{
	auto m_filePath = GetShaderFilePath();
	auto it = m_ShaderCache.find(m_filePath);
	if (it != m_ShaderCache.end())
	{
		return it->second;
	}
	else
	{
		Shader* shader = new Shader(m_filePath);
		//缓存shader以避免重复加载
		m_ShaderCache[m_filePath] = shader;
		return shader;
	}
}

void Material::ApplyMVPUniforms(glm::mat4 modelMatrix) const
{
	Shader* shader = GetShader();
	auto camera = Application::GetInstance()->renderer->camera;
	shader->SetUniformMat4f("u_Model", modelMatrix);
	shader->SetUniformMat4f("u_View", camera.ViewMatrix);
	shader->SetUniformMat4f("u_Projection", camera.ProjectionMatrix);
}

void Material::ApplyAllLightUniforms() const
{
	Shader* shader = GetShader();
	std::vector<std::shared_ptr<Light>>& lights = Application::GetInstance()->renderer->lights;
	int i;
	for (i = 0; i < lights.size(); i++) {
		lights[i]->SetToShader(shader, i);
	}
	for (; i < MAX_LIGHT_COUNT; i++)
	{
		Light::RemoveLight(shader, i);
	}
}