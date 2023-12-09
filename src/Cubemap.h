#pragma once

#include <iostream>
#include <format>
#include "stb_image.h"
#include "GLCALL.h"
#include <vector>

class Cubemap
{
private:
	std::vector<std::string> faces;
	unsigned int m_id = 0;
public:
	Cubemap() {
		GLCALL(glGenTextures(1, &m_id));
	}

	~Cubemap() {
		std::cout << "Release Cubemap(" << this->m_id << "): " << std::endl;
		GLCALL(glDeleteTextures(1, &m_id));
	}

	void Bind() const {
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_id));
	}

	void Unbind() const {
		GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}

	/// <summary>
	/// 加载立方体贴图
	/// order:
	/// +X (right)
	/// -X (left)
	/// +Y (top)
	/// -Y (bottom)
	/// +Z (front) 
	/// -Z (back)
	/// </summary>
	void Load(std::vector<std::string> faces) {
		std::cout << "Load cubemap(" << this->m_id << "):" << std::endl;
		Bind();
		this->faces = faces;
		int width, height, channels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			std::string path = faces[i];
			std::cout << " Load cubemap texture:" << path << std::endl;
			unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
			if (data)
			{
				GLenum format = GL_NONE;
				switch (channels) {
				case 1:
					format = GL_RED;
					break;
				case 2:
					format = GL_RG;
					break;
				case 3:
					format = GL_RGB;
					break;
				case 4:
					format = GL_RGBA;
					break;
				default:
					std::cout << "Load texture \"" << path << "\" have unexpected number of channels: " << channels << std::endl;
					break;
				}
				if (format != GL_NONE)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			}
			stbi_image_free(data);
		}
		setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		Unbind();
	}

	/// <summary>
	/// 设置参数，设置前需要绑定
	/// </summary>
	/// <param name="pname"></param>
	/// <param name="param"></param>
	void setParameter(unsigned int pname, unsigned int param) {
		GLCALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, pname, param));
	}

	void SetUnit(unsigned int unit) const {
		GLCALL(glBindTextureUnit(unit, m_id));
		//下面为旧的方法也可以用
		//GLCALL(glActiveTexture(GL_TEXTURE0 + unit));
		//GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, m_id));
	}

	static void UnsetUnit(unsigned int unit) {
		GLCALL(glBindTextureUnit(unit, 0));
		//下面为旧的方法也可以用
		//GLCALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}
};
