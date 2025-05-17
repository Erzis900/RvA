#pragma once
#include "atlas/Atlas.h"

struct AnimationData {
	const SpriteInfo* spriteInfo{};
	float frameTime{};
	int loop{-1};
	bool reverse{false};
};

class Animation {
public:
	Animation();
	Animation(const SpriteInfo* spriteInfo, float frameTime, int loop = -1, bool reverse = false);

	void update(float dt);

	int getCurrentFrame() const {
		return m_currentFrame;
	}

	void setLoop(int loop);
	bool isOver() const;
	void restart();
	const SpriteInfo* getSpriteInfo() const;

	static Animation createAnimation(AnimationData animationData);

private:
	const SpriteInfo* m_spriteInfo{};
	float m_frameTime{};
	int m_currentFrame{};
	float m_elapsedTime{};
	int m_loop{-1};		 // -1 = infinite loop, N = execute the animation N times
	int m_startLoop{-1}; // -1 = infinite loop, N = execute the animation N times
	int m_step{1};		 // 1 = forward, -1 = backward
	int m_lastFrame{};
	int m_startFrame{};
};
