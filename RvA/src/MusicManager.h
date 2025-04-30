#pragma once
#include <raylib.h>

class MusicManager
{
public:
	// https://github.com/raysan5/raylib/wiki/Using-raylib-with-Cpp#shared-gpu-resources-in-constructors-and-destructors
	// the reason I'm not loading resources in the constructor is because I don't understand how to make it work with raylib
	// more info in the link above

	~MusicManager();

	void load();
	void updateStream();

	void play(Music& music);
	void stop(Music& music);

	Music& getMenuMusic() { return m_menuMusic; }
	Music& getGameMusic() { return m_gameMusic; }
	Music& getLostMusic() { return m_lostMusic; }
private:
	Music m_menuMusic;
	Music m_gameMusic;
	Music m_lostMusic;

	Music m_currentMusic;
};