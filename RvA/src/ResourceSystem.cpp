#include "ResourceSystem.h"

void ResourceSystem::loadResources() {
	// Shaders
	addShader("highlight", LoadShader(nullptr, "assets/shaders/highlight.fs"));
	Shader* shader = getShader("highlight");
	int hlColorLoc = GetShaderLocation(*shader, "highlightColor");
	Vector4 glow = {0.25f, 0.25f, 0.25f, 0.25f};
	SetShaderValue(*shader, hlColorLoc, &glow, SHADER_UNIFORM_VEC4);

	// I will convert from wav to mp3 in the end once everything is finalized
	addMusic("menu", LoadMusicStream("assets/audio/music/menu.mp3"));
	addMusic("level1", LoadMusicStream("assets/audio/music/level1.mp3"));
	addMusic("level2", LoadMusicStream("assets/audio/music/level2.mp3"));
	addMusic("lost", LoadMusicStream("assets/audio/music/lost.mp3"));

	// UI
	addSound("button_click", LoadSound("assets/audio/sfx/ui/button_click.wav"), 0.2f);
	addSound("button_hover", LoadSound("assets/audio/sfx/ui/button_hover.wav"), 1.f);
	addSound("switch", LoadSound("assets/audio/sfx/ui/switch.wav"), 0.4f);

	// Alien
	addSound("alien_death", LoadSound("assets/audio/sfx/alien/death.wav"), 0.4f);
	addSound("alien_hit", LoadSound("assets/audio/sfx/alien/hit.wav"), 0.4f);
	addSound("alien_shock", LoadSound("assets/audio/sfx/alien/shock.wav"), 0.4f);

	// Robot
	addSound("robot_death", LoadSound("assets/audio/sfx/robot/death.wav"), 0.4f);
	addSound("robot_hit", LoadSound("assets/audio/sfx/robot/hit.wav"), 0.4f);
	addSound("robot_place", LoadSound("assets/audio/sfx/robot/place.wav"), 0.8f);

	// Portal
	addSound("portal_open", LoadSound("assets/audio/sfx/portal/portal_open.wav"), 0.8f);
	addSound("teleporting", LoadSound("assets/audio/sfx/portal/teleporting.wav"), 0.8f);

	// Bullet
	addSound("laser_shoot", LoadSound("assets/audio/sfx/bullet/laser.wav"), 0.2f);
	addSound("shooter_bullet", LoadSound("assets/audio/sfx/bullet/shooter.wav"), 0.8f);
	addSound("catapult_bullet", LoadSound("assets/audio/sfx/bullet/catapult.wav"), 0.8f);
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
