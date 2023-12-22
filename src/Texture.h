#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class Texture {
public:
	Texture();
	~Texture();
	void setParameter(unsigned int pname, unsigned int param);
	void setParameter(unsigned int pname, float* param);
	void SetUnit(unsigned int unit) const;
	void UnsetUnit(unsigned int unit) const;
	void bind() const;
	void unbind() const;
	static std::shared_ptr<Texture> Get(unsigned char r, unsigned char g, unsigned char b);
	static std::shared_ptr<Texture> Get(const std::string& path, bool flip = false, bool sRGB = true, bool generateMipmap = true);
	inline std::string GetPath() { return this->path; }
	inline unsigned int GetId() const { return this->m_id; }

	static void ClearCache() { m_TextureCache.clear(); }
	static bool enableSRGB; //调试用
private:
	int loadId = 0; //调试用
	std::string path;
	unsigned int m_id = 0;
	int width = 0, height = 0, channels = 0;
	unsigned char* data = nullptr;

	void Init();
	void Load(const std::string& path, bool flip = false, bool sRGB = true, bool generateMipmap = true);  //这个方法会改变path，但path不能随便修改，因为它是贴图缓存的key
	void SetPureColor(unsigned char r, unsigned char g, unsigned char b); //同上
	static bool FindOrCreate(const std::string& path, std::shared_ptr<Texture>& texture);
	static std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureCache;
};