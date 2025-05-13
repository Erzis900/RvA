#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

class ResourceSystem {
public:
	void loadResources();
	void unloadResources();

	Shader getShader(const std::string& id);

private:
	void addShader(std::string id, Shader shader);

	std::unordered_map<std::string, Shader> m_shaders;
};
