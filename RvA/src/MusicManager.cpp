#include "MusicManager.h"

#include "Config.h"
#include "ResourceSystem.h"
#include "constants.h"

MusicManager::MusicManager(Config& config, ResourceSystem& resourceSystem) : m_config(config), m_resourceSystem(resourceSystem) {
	SetMasterVolume(m_config.options.masterVolume);
}

void MusicManager::playMusic(const std::string& name) {
	if (m_config.options.isMusicEnabled) {
		Music* music = m_resourceSystem.getMusic(name);

		if (music && !IsMusicStreamPlaying(*music)) {
			if (m_currentMusic) {
				StopMusicStream(*m_currentMusic);
			}
			m_currentMusic = *music;
			SetMasterVolume(m_config.options.masterVolume);
			SetMusicVolume(*m_currentMusic, m_config.options.musicVolume);
			PlayMusicStream(*m_currentMusic);
		}
	}
}

void MusicManager::playSound(const std::string& name) {
	if (m_config.options.isSoundEnabled) {
		Sound* sound = m_resourceSystem.getSound(name);
		if (sound) {
			SetSoundVolume(*sound, m_config.options.sfxVolume);
			PlaySound(*sound);
		}
	}
}

void MusicManager::stopMusic() {
	if (m_config.options.isMusicEnabled) {
		StopMusicStream(*m_currentMusic);
	}
}

void MusicManager::updateStream() {
	if (m_config.options.isMusicEnabled && m_currentMusic) {
		UpdateMusicStream(*m_currentMusic);
	}
}

bool MusicManager::isMusicEnabled() const {
	return m_config.options.isMusicEnabled;
}

bool MusicManager::isSoundEnabled() const {
	return m_config.options.isSoundEnabled;
}

void MusicManager::toggleMusic() {
	if (m_config.options.isMusicEnabled) {
		muteMusic();
	} else {
		unmuteMusic();
	}
}

void MusicManager::toggleSound() {
	if (m_config.options.isSoundEnabled) {
		muteSound();
	} else {
		unmuteSound();
	}
}

void MusicManager::muteMusic() {
	if (m_config.options.isMusicEnabled) {
		m_config.options.isMusicEnabled = false;
		StopMusicStream(*m_currentMusic);
	}
}

void MusicManager::muteSound() {
	if (m_config.options.isSoundEnabled) {
		m_config.options.isSoundEnabled = false;
	}
}

void MusicManager::unmuteMusic() {
	if (!m_config.options.isMusicEnabled) {
		m_config.options.isMusicEnabled = true;
	}
}

void MusicManager::unmuteSound() {
	if (!m_config.options.isSoundEnabled) {
		m_config.options.isSoundEnabled = true;
	}
}

void MusicManager::setMasterVolume(float volume) {
	m_config.options.masterVolume = volume;
	SetMasterVolume(volume);
}

void MusicManager::setMusicVolume(float volume) {
	m_config.options.musicVolume = volume;
	if (m_currentMusic) {
		SetMusicVolume(*m_currentMusic, volume);
	}
}

void MusicManager::setSoundVolume(float volume) {
	m_config.options.sfxVolume = volume;
}

float MusicManager::getMasterVolume() const {
	return m_config.options.masterVolume;
}

float MusicManager::getMusicVolume() const {
	return m_config.options.musicVolume;
}

float MusicManager::getSoundVolume() const {
	return m_config.options.sfxVolume;
}
