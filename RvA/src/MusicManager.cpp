#include "MusicManager.h"

#include "Config.h"
#include "ResourceSystem.h"
#include "constants.h"

MusicManager::MusicManager(Config& config, ResourceSystem& resourceSystem) : m_config(config), m_resourceSystem(resourceSystem) {}

void MusicManager::playMusic(const std::string& name) {
	if (m_config.options.isMusic) {
		Music& music = m_resourceSystem.getMusic(name);

		if (!IsMusicStreamPlaying(music)) {
			if (m_currentMusic) {
				StopMusicStream(*m_currentMusic);
			}
			m_currentMusic = music;
			PlayMusicStream(*m_currentMusic);
		}
	}
}

void MusicManager::playSound(const std::string& name) {
	if (m_config.options.isSound) {
		Sound& sound = m_resourceSystem.getSound(name);

		PlaySound(sound);
	}
}

void MusicManager::stopMusic() {
	if (m_config.options.isMusic) {
		StopMusicStream(*m_currentMusic);
	}
}

void MusicManager::updateStream() {
	if (m_config.options.isMusic && m_currentMusic) {
		UpdateMusicStream(*m_currentMusic);
	}
}
