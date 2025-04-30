#include "MusicManager.h"

MusicManager::~MusicManager()
{
	UnloadMusicStream(m_menuMusic);
	UnloadMusicStream(m_gameMusic);
	UnloadMusicStream(m_lostMusic);
}

void MusicManager::load()
{
	// TODO make sure all the music files have consistent formats
	m_menuMusic = LoadMusicStream("sfx/menu.wav");
	m_gameMusic = LoadMusicStream("sfx/game.mp3");
	m_lostMusic = LoadMusicStream("sfx/lost.mp3");
}

void MusicManager::play(Music& music)
{
	m_currentMusic = music; 
	PlayMusicStream(m_currentMusic);
}

void MusicManager::stop(Music& music)
{
	StopMusicStream(music);
}

void MusicManager::updateStream()
{
	UpdateMusicStream(m_currentMusic);
}