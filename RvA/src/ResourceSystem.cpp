#include "ResourceSystem.h"

void ResourceSystem::loadResources() {
	addMusic("menu", LoadMusicStream("sfx/menu.wav"));
	addMusic("level1", LoadMusicStream("sfx/game.mp3"));
	addMusic("level2", LoadMusicStream("sfx/robots leaking.wav"));
	addMusic("lost", LoadMusicStream("sfx/lost.mp3"));

	addSound("button_click", LoadSound("sfx/buttonClick.wav"), 0.2f);
	addSound("laser_shoot", LoadSound("sfx/laserShoot.wav"), 0.2f);
	addSound("alien_death", LoadSound("sfx/alienDeath.wav"), 0.4f);
	addSound("switch", LoadSound("sfx/switch.wav"), 0.4f);
}

void ResourceSystem::unloadResources() {
	for (auto& [id, shader] : m_shaders) {
		UnloadShader(shader);
	}
	m_shaders.clear();

	for (auto& [id, music] : m_musics) {
		UnloadMusicStream(music);
	}
	m_musics.clear();

	for (auto& [id, sound] : m_sounds) {
		UnloadSound(sound);
	}
	m_sounds.clear();
}

Shader* ResourceSystem::getShader(const std::string& id) {
	auto it = m_shaders.find(id);
	if (it != m_shaders.end()) {
		return &it->second;
	}
	return nullptr;
}

Music* ResourceSystem::getMusic(const std::string& id) {
	auto it = m_musics.find(id);
	if (it != m_musics.end()) {
		return &it->second;
	}
	return nullptr;
}

Sound* ResourceSystem::getSound(const std::string& id) {
	auto it = m_sounds.find(id);
	if (it != m_sounds.end()) {
		return &it->second;
	}
	return nullptr;
}

void ResourceSystem::addShader(std::string id, Shader shader) {
	m_shaders.insert({std::move(id), std::move(shader)});
}

void ResourceSystem::addMusic(std::string id, Music music) {
	m_musics.insert({std::move(id), std::move(music)});
}

void ResourceSystem::addSound(std::string id, Sound sound, float volume) {
	SetSoundVolume(sound, volume);
	m_sounds.insert({std::move(id), std::move(sound)});
}
