#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class Texture {
public:
	Texture();
	~Texture();
	void GenerateMipmap();
	void setParameter(unsigned int pname, unsigned int param);
	void SetUnit(unsigned int unit) const;
	void UnsetUnit(unsigned int unit) const;
	void bind() const;
	void unbind() const;
	static std::shared_ptr<Texture> Get(unsigned char r, unsigned char g, unsigned char b);
	static std::shared_ptr<Texture> Get(const std::string& path, bool flip = false, bool generateMipmap = true);
	inline std::string GetPath() { return this->path; }
	inline unsigned int GetId() const { return this->m_id; }

private:
	int loadId = 0; //调试用
	std::string path;
	unsigned int m_id = 0;
	int width = 0, height = 0, channels = 0;
	unsigned char* data = nullptr;

	void Init();
	void Load(const std::string& path, bool flip = false, bool generateMipmap = true);  //这个方法会改变path，但path不能随便修改，因为它是贴图缓存的key
	void SetPureColor(unsigned char r, unsigned char g, unsigned char b); //同上
	static bool FindOrCreate(const std::string& path, std::shared_ptr<Texture>& texture);
	static std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureCache;

	/*const unsigned char errorTextureData[5 * 5 * 3] = {
	255,   0,   0,   255, 255, 255,   255, 255, 255,   255, 255, 255,   255,   0,   0,
	255, 255, 255,   255,   0,   0,   255, 255, 255,   255,   0,   0,   255, 255, 255,
	255, 255, 255,   255, 255, 255,   255,   0,   0,   255, 255, 255,   255, 255, 255,
	255, 255, 255,   255,   0,   0,   255, 255, 255,   255,   0,   0,   255, 255, 255,
	255,   0,   0,   255, 255, 255,   255, 255, 255,   255, 255, 255,   255,   0,   0
	};*/
	//const unsigned char errorTextureData[5 * 5 * 3] = {
	//000, 255, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,
	//000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,
	//000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,
	//000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,
	//000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000,   000, 000, 000
	//};
	//const static int errorTextureWidth = 20;
	//const static int errorTextureChannels = 3;
	//static char errorTextureData[errorTextureWidth * errorTextureWidth * errorTextureChannels];
};