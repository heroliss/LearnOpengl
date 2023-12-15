#pragma once

#include "Material.h"
#include "Texture.h"
#include <string>
#include "Application.h"
#include "Light.h"
#include <assimp/material.h>
#include <assimp/types.h>
#include <format>
#include "Cubemap.h"

class BaseMaterial3D : public Material
{
public:
	glm::vec4 ObjectColor = glm::vec4(1, 1, 1, 1);
	glm::vec3 Ambient = glm::vec3(0.04f, 0.04f, 0.04f);
	glm::vec3 Emission_inside = glm::vec3(0, 0, 0);
	glm::vec3 Emission_outside = glm::vec3(0, 0, 0);

	std::shared_ptr<Texture> MainTexture = Texture::Get(255, 255, 255);
	std::shared_ptr<Texture> NormalTexture = Texture::Get(128, 128, 255);
	std::shared_ptr<Texture> SpecularTexture = Texture::Get(255, 255, 255);
	std::shared_ptr<Texture> HeightTexture = Texture::Get(255, 255, 255);
	glm::vec3& viewPos = Application::GetInstance()->renderer->camera.position;

	glm::vec3 SpecularColor = glm::vec3(0.5f, 0.5f, 0.5f); //高光反射颜色和强度
	float Shininess = 64; //高光反光度（范围）

	bool showDepth = false;
	glm::vec2 showDepthRange = glm::vec2(1, 500); //预设的深度图范围

	std::shared_ptr<Cubemap> cubemap; //环境立方体贴图（目前用于天空盒反射和折射，进阶可变为反射探针结果）

	bool enableRefract; //是否开启折射（目前仅支持折射天空盒，此时透明度恒为1）
	float refractiveIndex = 1.52; //折射率 (空气1.00 水1.33 冰1.309	玻璃1.52 钻石2.42)
	glm::vec3 refractColor = glm::vec3(1); //折射颜色和强度

	std::string GetShaderFilePath() const override
	{
		return "res/shaders/3D_basic.glsl";
	}

	void ApplyUniforms(glm::mat4 modelMatrix) const override
	{
		ApplyMVPUniforms(modelMatrix);
		ApplyAllLightUniforms();

		auto shader = GetShader();
		shader->SetUniform4f("u_objectColor", ObjectColor.r, ObjectColor.g, ObjectColor.b, ObjectColor.a);
		shader->SetUniform3f("u_ambient", Ambient.r, Ambient.g, Ambient.b);
		shader->SetUniform3f("u_emission_inside", Emission_inside.r, Emission_inside.g, Emission_inside.b);
		shader->SetUniform3f("u_emission_outside", Emission_outside.r, Emission_outside.g, Emission_outside.b);
		shader->SetUniform3f("u_specularColor", SpecularColor.r, SpecularColor.g, SpecularColor.b);
		shader->SetUniform1f("u_shininess", Shininess);
		shader->SetUniform3f("u_viewPos", viewPos.x, viewPos.y, viewPos.z);

		//纹理贴图
		shader->SetUniform1i("u_mainTexture", 0);
		if (MainTexture != nullptr) MainTexture->SetUnit(0); else MainTexture->UnsetUnit(0);
		shader->SetUniform1i("u_normalTexture", 1);
		if (NormalTexture != nullptr) NormalTexture->SetUnit(1); else NormalTexture->UnsetUnit(1);
		shader->SetUniform1i("u_specularTexture", 2);
		if (SpecularTexture != nullptr) SpecularTexture->SetUnit(2); else SpecularTexture->UnsetUnit(2);
		shader->SetUniform1i("u_heightTexture", 3);
		if (HeightTexture != nullptr) HeightTexture->SetUnit(3); else HeightTexture->UnsetUnit(3);

		//深度图
		shader->SetUniform1i("u_showDepth", showDepth); //是否显示深度图
		shader->SetUniform2f("u_showDepthRange", showDepthRange.x, showDepthRange.y); //深度图的范围

		//环境立方体贴图，用于反射和折射
		shader->SetUniform1i("u_cubemap", 4);
		if (cubemap != nullptr) cubemap->SetUnit(4); else Cubemap::UnsetUnit(4);

		//折射
		shader->SetUniform1i("u_enableRefract", enableRefract);
		shader->SetUniform1f("u_refractiveIndex", refractiveIndex); //折射率
		shader->SetUniform3f("u_refractColor", refractColor.x, refractColor.y, refractColor.z); //折射颜色和强度
	}

	/// <summary>
	/// 从aiMaterial设置属性和贴图，设置后要调用ApplyUniforms来生效
	/// </summary>
	/// <param name="mat"></param>
	/// <param name="directory">加载贴图的根目录，必须以/结尾</param>
	void SetFromAiMaterial(aiMaterial* mat, std::string directory, bool useNormalMap = true, bool useDiffuseTexture = true, bool useSpecularTexture = true)
	{
		aiColor3D color;
		aiString texturePath;
		//环境光
		if (mat->GetTextureCount(aiTextureType_AMBIENT) == 0)
		{
			//mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
			//Ambient = glm::vec3(color.r, color.g, color.b);
		}
		else {
			std::cout << "警告：材质提供了环境光贴图，但该shader没有实现环境光贴图的处理!" << std::endl;
		}
		//漫反射
		if (useDiffuseTexture == false || mat->GetTextureCount(aiTextureType_DIFFUSE) == 0)
		{
			//不透明度
			float opacity = 1;
			mat->Get(AI_MATKEY_OPACITY, opacity);
			mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			ObjectColor = glm::vec4(color.r, color.g, color.b, opacity);
			MainTexture = Texture::Get(255, 255, 255);
		}
		else
		{
			//ObjectColor = glm::vec4(1);
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath); //这里只获取了第一个贴图路径
			MainTexture = Texture::Get(directory + texturePath.C_Str());
			//std::cout << "Apply diffuse texture: " << MainTexture->path << std::endl;
		}

		//镜面反射
		if (useSpecularTexture == false || mat->GetTextureCount(aiTextureType_SPECULAR) == 0)
		{
			//mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
			//SpecularColor = glm::vec3(color.r, color.g, color.b);
			SpecularTexture = Texture::Get(255, 255, 255);
		}
		else {
			//SpecularColor = glm::vec3(1);
			mat->GetTexture(aiTextureType_SPECULAR, 0, &texturePath);
			SpecularTexture = Texture::Get(directory + texturePath.C_Str(), false, false);
			//std::cout << "Apply specular texture: " << SpecularTexture->path << std::endl;
		}

		//法线贴图
		if (useNormalMap == false || mat->GetTextureCount(aiTextureType_HEIGHT) == 0) //注意：这里是aiTextureType_HEIGHT而不是aiTextureType_NORMALS，谁也不知道为什么...
		{
			NormalTexture = Texture::Get(128, 128, 255);
		}
		else {
			mat->GetTexture(aiTextureType_HEIGHT, 0, &texturePath);
			NormalTexture = Texture::Get(directory + texturePath.C_Str(), false, false);
			//std::cout << std::format("aiMaterial({}): Apply normal texture: {}\n", mat->GetName().C_Str(), NormalTexture->GetPath());
		}

		//高度图 //TODO
	}
};



