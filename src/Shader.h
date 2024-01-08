#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "glm/glm.hpp"

struct ShaderProgramSources
{
	std::string Useless;
	std::string CommonSource;
	std::string VertexSource;
	std::string FragmentSource;
	std::string GeometrySource;
};

class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	Shader(const std::string& filePath);
	~Shader();

	void Bind() const;
	void UnBind() const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform2i(const std::string& name, int v0, int v1);
	void SetUniform1iv(const std::string& name, int count, const int* value);
	void SetUniform1fv(const std::string& name, int count, const float* value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	void SetUniformMat4fv(const std::string& name, const glm::mat4(&matrix)[], unsigned int count);

	unsigned int GetId() { return m_RendererID; }

	std::shared_ptr<ShaderProgramSources> sources;

private:
	int GetUniformLocation(const std::string& name) const;
	ShaderProgramSources ParseShader(const std::string& filePath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader();
};

