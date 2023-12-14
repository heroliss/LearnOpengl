#include "Texture.h"
#include <iostream>
#include <format>
#include "stb_image.h"
#include "GLCALL.h"

std::unordered_map<std::string, std::shared_ptr<Texture>> Texture::m_TextureCache;
static int loadCount = 0; //调试用，加载总数
const static int errorTextureWidth = 16; //TODO: 为什么必须是4的整数被？否则渲染错误
const static int errorTextureChannels = 3;
static char errorTextureData[errorTextureWidth * errorTextureWidth * errorTextureChannels];
static bool errorTextureInited = false;
void Texture::Init()
{
	GLCALL(glGenTextures(1, &m_id));
	//构建一个红色的×贴图表示没有成功加载的贴图
	if (errorTextureInited == false) {
		errorTextureInited = true;
		for (int i = 0; i < errorTextureWidth; i++)
		{
			for (int j = 0; j < errorTextureWidth; j++)
			{
				int c = i * errorTextureWidth + j;
				int p = c * errorTextureChannels;
				if (i == j || i == errorTextureWidth - 1 - j) {
					errorTextureData[p] = 255;
					for (int k = 1; k < errorTextureChannels; k++)
					{
						errorTextureData[p + k] = 0;
					}
				}
				else {
					for (int k = 0; k < errorTextureChannels; k++)
					{
						errorTextureData[p + k] = 255;
					}
				}
			}
		}
	}
}
Texture::Texture() {
	Init();
}

Texture::~Texture() {
	std::cout << loadId << " Release texture(" << this->m_id << "): " << path << std::endl;
	GLCALL(glDeleteTextures(1, &m_id));
	free(data);
}

void Texture::SetPureColor(unsigned char r, unsigned char g, unsigned char b) {
	loadCount++; //调试用
	loadId = loadCount;

	path = std::format("#pureColor: {} {} {}", r, g, b);
	std::cout << loadId << " Set pure color texture(" << this->m_id << "):    " << path << std::endl;
	width = 1;
	height = 1;
	channels = 3;

	free(data);
	data = new unsigned char[width * height * 3] {r, g, b};
	bind();
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
	unbind();
}

/// <summary>
/// 仅加载贴图，不会缓存
/// </summary>
/// <param name="path"></param>
void Texture::Load(const std::string& path, bool flip, bool generateMipmap) {
	loadCount++; //调试用
	loadId = loadCount;
	std::cout << loadId << " Load texture(" << this->m_id << "):    " << path << std::endl;

	//加载图片
	this->path = path;
	stbi_set_flip_vertically_on_load(flip);
	auto loadedData = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!loadedData) {
		std::cout << "Load texture failed: " << path << std::endl;
		//设置默认加载失败的警示贴图
		width = errorTextureWidth;
		height = errorTextureWidth;
		channels = errorTextureChannels;
		int size = std::size(errorTextureData);
		free(data);
		data = new unsigned char[size];
		std::copy(errorTextureData, errorTextureData + size, data);
	}
	else {
		free(data);;
		data = loadedData;
	}

	GLenum format;
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
		return;
	}

	bind();
	GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));

	if (generateMipmap)
	{
		GLCALL(glGenerateMipmap(GL_TEXTURE_2D));
		setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else
	{
		setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	unbind();
}

/// <summary>
/// 设置参数，设置前需要绑定
/// </summary>
/// <param name="pname"></param>
/// <param name="param"></param>
void Texture::setParameter(unsigned int pname, unsigned int param) {
	GLCALL(glTexParameteri(GL_TEXTURE_2D, pname, param));
}

void Texture::SetUnit(unsigned int unit) const {
	GLCALL(glBindTextureUnit(unit, m_id));
}

void Texture::UnsetUnit(unsigned int unit) const {
	GLCALL(glBindTextureUnit(unit, 0));
}

void Texture::bind() const {
	GLCALL(glBindTexture(GL_TEXTURE_2D, m_id));
}

void Texture::unbind() const {
	GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
}

std::shared_ptr<Texture> Texture::Get(unsigned char r, unsigned char g, unsigned char b)
{
	std::string path = std::format("#pureColor: {} {} {}", r, g, b);
	std::shared_ptr<Texture> texture;
	if (FindOrCreate(path, texture) == false) {
		texture->SetPureColor(r, g, b);
	}
	return texture;
}

/// <summary>
/// 从缓存中获取贴图，如果没有则加载并添加到缓存  
/// TODO:目前缓存还没有自动释放的功能，所有贴图会一直缓存
/// </summary>
/// <param name="path"></param>
/// <param name="flip"></param>
/// <returns></returns>
std::shared_ptr<Texture> Texture::Get(const std::string& path, bool flip, bool generateMipmap)
{
	std::shared_ptr<Texture> texture;
	if (FindOrCreate(path, texture) == false)
	{
		texture->Load(path, flip, generateMipmap);
	}
	return texture;
}

/// <summary>
/// 从缓存中获取或创建贴图
/// </summary>
/// <param name="path"></param>
/// <param name="texture"></param>
/// <returns>是否在缓存中找到贴图</returns>
bool Texture::FindOrCreate(const std::string& path, std::shared_ptr<Texture>& texture)
{
	// 在缓存中查找是否有对应的键值对
	auto it = m_TextureCache.find(path);
	if (it != m_TextureCache.end())
	{
		texture = it->second;
		return true;
		//以下是弱引用的方式缓存
		//if (it->second.expired() == false)
		//{
		//	auto texture = it->second.lock();
		//	if (texture != nullptr)
		//		return texture;
		//}
		//else
		//{
		//	std::cout << "expired texture: " << path << std::endl;
		//}
	}

	// 创建一个新的 Texture 实例
	texture = std::make_shared<Texture>();

	// 将该实例存储到缓存中
	m_TextureCache[path] = texture;

	return false;
}
