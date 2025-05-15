#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

class ResourceSystem {
public:
	void loadResources();
	void unloadResources();

	Shader& getShader(const std::string& id);
	Music& getMusic(const std::string& id);
	Sound& getSound(const std::string& id);

private:
	void addShader(std::string id, Shader shader);
	void addMusic(std::string id, Music music);
	void addSound(std::string id, Sound music, float volume = 1.f);

	std::unordered_map<std::string, Shader> m_shaders;
	std::unordered_map<std::string, Music> m_musics;
	std::unordered_map<std::string, Sound> m_sounds;
};
