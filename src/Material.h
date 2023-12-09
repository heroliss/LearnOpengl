#pragma once

#include "Shader.h"
#include <string>
#include <unordered_map>
#include <iostream>

class Material
{
public:
	//这里必须有虚的析构方法定义，才能使子类的析构方法被正确调用到！否则会发生内存泄露
	virtual ~Material() {
		//std::cout << "销毁Material！！！！！！！！！" << std::endl;
	}

	bool IsTransparent;

	Shader* GetShader() const;
	void ApplyMVPUniforms(glm::mat4 modelMatrix) const;
	void ApplyAllLightUniforms() const;
	virtual void ApplyUniforms(glm::mat4 modelMatrix) const = 0;
	virtual std::string GetShaderFilePath() const = 0;

private:
	static std::unordered_map<std::string, Shader*> m_ShaderCache;
};

