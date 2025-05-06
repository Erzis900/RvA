#pragma once
#include "atlas/Atlas.h"

struct AnimationData {
	const SpriteInfo* spriteInfo{};
	float frameTime{};
	int loop{-1};
};

class Animation {
public:
	Animation();
	Animation(const SpriteInfo* spriteInfo, float frameTime, int loop = -1);

	void update(float dt);

	int getCurrentFrame() const {
		return m_currentFrame;
	}

	void setLoop(int loop);
	bool isOver() const;
	const SpriteInfo* getSpriteInfo() const;

	static Animation createAnimation(AnimationData animationData);

private:
	const SpriteInfo* m_spriteInfo{};
	float m_frameTime{};
	int m_currentFrame{};
	float m_elapsedTime{};
	int m_loop{-1}; // -1 = infinite loop, N = execute the animation N times
};
