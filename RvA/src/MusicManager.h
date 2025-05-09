#pragma once
#include <raylib.h>

class Config;

class MusicManager {
public:
	// https://github.com/raysan5/raylib/wiki/Using-raylib-with-Cpp#shared-gpu-resources-in-constructors-and-destructors
	// the reason I'm not loading resources in the constructor is because I don't understand how to make it work with raylib
	// more info in the link above

	MusicManager(Config& config);
	~MusicManager();

	void load();
	void updateStream();

	void play(Music& music);
	void play(Sound& sound);
	void stop(Music& music);

	Music& getMenuMusic() {
		return m_menuMusic;
	}

	Music& getGameMusic() {
		return m_gameMusic;
	}

	Music& getLostMusic() {
		return m_lostMusic;
	}

	// TODO add Music+Sound Volume slider in Options screen
	Sound& getButtonClick() {
		SetSoundVolume(s_buttonClick, 0.2f);
		return s_buttonClick;
	}

	Sound& getLaserShoot() {
		return s_laserShoot;
	}

	Sound& getLongLaser() {
		return s_longLaser;
	}

	Sound& getAlienDeath() {
		return s_alienDeath;
	}

private:
	Config& m_config;

	Music m_menuMusic;
	Music m_gameMusic;
	Music m_lostMusic;

	Music m_currentMusic;

	Sound s_buttonClick;
	Sound s_laserShoot;
	Sound s_longLaser;
	Sound s_alienDeath;
};
