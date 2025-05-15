#include "ResourceSystem.h"

void ResourceSystem::loadResources() {
	addShader("hole", LoadShader(nullptr, "assets/shaders/hole_shader.fs"));

	addMusic("menu", LoadMusicStream("sfx/menu.wav"));
	addMusic("game", LoadMusicStream("sfx/game.mp3"));
	addMusic("lost", LoadMusicStream("sfx/lost.mp3"));

	addSound("button_click", LoadSound("sfx/buttonClick.wav"), 0.2f);
	addSound("laser_shoot", LoadSound("sfx/laserShoot.wav"), 0.2f);
	addSound("alien_death", LoadSound("sfx/alienDeath.wav"), 0.4f);
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

Shader& ResourceSystem::getShader(const std::string& id) {
	auto it = m_shaders.find(id);
	if (it != m_shaders.end()) {
		return it->second;
	}
}

Music& ResourceSystem::getMusic(const std::string& id) {
	auto it = m_musics.find(id);
	if (it != m_musics.end()) {
		return it->second;
	}
}

Sound& ResourceSystem::getSound(const std::string& id) {
	auto it = m_sounds.find(id);
	if (it != m_sounds.end()) {
		return it->second;
	}
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
