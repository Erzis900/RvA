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

	bool isMusicEnabled() const;
	bool isSoundEnabled() const;
	void toggleMusic();
	void toggleSound();
	void muteMusic();
	void muteSound();
	void unmuteMusic();
	void unmuteSound();
	void setMasterVolume(float volume);
	void setMusicVolume(float volume);
	void setSoundVolume(float volume);
	float getMasterVolume() const;
	float getMusicVolume() const;
	float getSoundVolume() const;

	void playMusic(const std::string& name);
	void stopMusic();
	void playSound(const std::string& name);

private:
	Config& m_config;
	ResourceSystem& m_resourceSystem;

	std::optional<Music> m_currentMusic;
};
