#include "MusicManager.h"

#include "constants.h"

MusicManager::~MusicManager() {
	UnloadMusicStream(m_menuMusic);
	UnloadMusicStream(m_gameMusic);
	UnloadMusicStream(m_lostMusic);
}

void MusicManager::load() {
	// TODO make sure all the music files have consistent formats
	m_menuMusic = LoadMusicStream("sfx/menu.wav");
	m_gameMusic = LoadMusicStream("sfx/game.mp3");
	m_lostMusic = LoadMusicStream("sfx/lost.mp3");

	// is there a shorter code than game.getMusicManager().play(game.getMusicManager().getButtonClick()); to play a sound?
	// something like playMySound("buttonClick"); without needing to pass &game every time?
	s_buttonClick = LoadSound("sfx/buttonClick.wav");
	s_laserShoot = LoadSound("sfx/laserShoot.wav");
	s_longLaser = LoadSound("sfx/longLaser.wav");
	s_alienDeath = LoadSound("sfx/alienDeath.wav");
}

void MusicManager::play(Music& music) {
	if (MUSIC) {
		if (!IsMusicStreamPlaying(music)) {
			StopMusicStream(m_currentMusic);
			m_currentMusic = music;
			PlayMusicStream(m_currentMusic);
		}
	}
}

void MusicManager::play(Sound& sound) {
	if (SOUND) {
		PlaySound(sound);
	}
}

void MusicManager::stop(Music& music) {
	if (MUSIC) {
		StopMusicStream(music);
	}
}

void MusicManager::updateStream() {
	if (MUSIC) {
		UpdateMusicStream(m_currentMusic);
	}
}
