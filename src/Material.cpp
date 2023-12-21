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