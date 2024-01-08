#include "Shader.h"
#include "GLCALL.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(const std::string& filePath)
	: m_FilePath(filePath), m_RendererID(0)
{
	this->sources = std::make_shared<ShaderProgramSources>(ParseShader(filePath));

	//std::cout << "------------ Useless ------------" << std::endl;
	//std::cout << source.Useless << std::endl;
	//std::cout << "------------ Common ------------" << std::endl;
	//std::cout << source.CommonSource << std::endl;
	//std::cout << "------------ Vertex ------------" << std::endl;
	//std::cout << source.VertexSource << std::endl;
	//std::cout << "----------- Fragment -----------" << std::endl;
	//std::cout << source.FragmentSource << std::endl;
	//std::cout << "----------- Geometry -----------" << std::endl;
	//std::cout << source.GeometrySource << std::endl;

	CreateShader();
}

Shader::~Shader()
{
	std::cout << "销毁shader！！！ path: " << m_FilePath << std::endl;
	GLCALL(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	GLCALL(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
	GLCALL(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCALL(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform2i(const std::string& name, int v0, int v1)
{
	GLCALL(glUniform2i(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform1iv(const std::string& name, int count, const int* value)
{
	GLCALL(glUniform1iv(GetUniformLocation(name), count, value));
}

void Shader::SetUniform1fv(const std::string& name, int count, const float* value)
{
	GLCALL(glUniform1fv(GetUniformLocation(name), count, value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCALL(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, float v0, float v1)
{
	GLCALL(glUniform2f(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCALL(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCALL(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCALL(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformMat4fv(const std::string& name, const glm::mat4 (&matrix)[], unsigned int count)
{
	GLCALL(glUniformMatrix4fv(GetUniformLocation(name), count, GL_FALSE, &matrix[0][0][0]));
}

int Shader::GetUniformLocation(const std::string& name) const
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}
	int location = -1;
	GLCALL(location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1) {
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	}
	m_UniformLocationCache[name] = location;
	return location;
}

ShaderProgramSources Shader::ParseShader(const std::string& filePath)
{
	m_FilePath = filePath;
	std::ifstream stream(filePath);
	if (stream.is_open()) {
		enum class ShaderType
		{
			NONE = 0, COMMON = 1, VERTEX = 2, FRAGMENT = 3, GEOMETRY = 4,
		};
		std::string line;
		std::stringstream ss[5];
		ShaderType type = ShaderType::NONE;
		while (getline(stream, line))
		{
			if (line.find("##common") != std::string::npos) {
				type = ShaderType::COMMON;
			}
			else if (line.find("##shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
				else if (line.find("geometry") != std::string::npos)
					type = ShaderType::GEOMETRY;
			}
			else {
				ss[(int)type] << line << '\n';
			}
		}
		stream.close();
		return { ss[0].str(), ss[1].str(), ss[2].str(), ss[3].str(), ss[4].str() };
	}
	return {};
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id;
	GLCALL(id = glCreateShader(type));
	const char* src = source.c_str();
	GLCALL(glShaderSource(id, 1, &src, nullptr));
	GLCALL(glCompileShader(id));

	//检查编译结果
	int result;
	GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)_malloca(length * sizeof(char));
		GLCALL(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << m_FilePath << std::endl;
		std::cout << message << std::endl;
		GLCALL(glDeleteShader(id));
		return 0;
	}
	return id;
}

/// <summary>
/// 修改shader后调用该方法重新编译生成program
/// </summary>
unsigned int Shader::CreateShader()
{
	m_RendererID = 0;
	GLCALL(glDeleteProgram(m_RendererID));

	ShaderProgramSources& sources = *this->sources;
	if (sources.Useless.empty() == false) {
		std::cout << "Warning: Useless code found in shader: " << m_FilePath << std::endl << sources.Useless;
	}
	unsigned int program;
	GLCALL(program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, sources.CommonSource + sources.VertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, sources.CommonSource + sources.FragmentSource);
	unsigned int gs = -1;
	if (sources.GeometrySource.empty() == false) {
		gs = CompileShader(GL_GEOMETRY_SHADER, sources.CommonSource + sources.GeometrySource);
		GLCALL(glAttachShader(program, gs));
	}

	GLCALL(glAttachShader(program, vs));
	GLCALL(glAttachShader(program, fs));
	GLCALL(glLinkProgram(program));
	GLCALL(glValidateProgram(program));

	GLCALL(glDeleteShader(vs));
	GLCALL(glDeleteShader(fs));
	if (gs != -1) GLCALL(glDeleteShader(gs));

	//检查链接结果
	int result;
	GLCALL(glGetProgramiv(program, GL_LINK_STATUS, &result));
	if (result == GL_FALSE)
	{
		std::cout << "Failed to link shader: " << m_FilePath << std::endl;
		int length;
		GLCALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
		if (length > 0)
		{
			char* message = (char*)_malloca(length * sizeof(char)); //使用_malloca在栈上分配内存，无需释放
			GLCALL(glGetProgramInfoLog(program, length, &length, message));
			std::cout << message << std::endl;
		}
		GLCALL(glDeleteProgram(program));
		m_RendererID = 0;
	}

	m_RendererID = program;
	return m_RendererID;
}