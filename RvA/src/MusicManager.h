#pragma once

#include <optional>
#include <raylib.h>
#include <string>

class Config;
class ResourceSystem;

class MusicManager {
public:
	MusicManager(Config& config, ResourceSystem& resourceManager);

	void updateStream();

	void playMusic(const std::string& name);
	void stopMusic();
	void playSound(const std::string& name);

private:
	Config& m_config;
	ResourceSystem& m_resourceSystem;

	std::optional<Music> m_currentMusic;
};
