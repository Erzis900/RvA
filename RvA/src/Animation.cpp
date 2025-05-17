#include "Animation.h"

#include <cassert>

Animation Animation::createAnimation(AnimationData animationData) {
	if (animationData.spriteInfo) {
		return Animation(animationData.spriteInfo, animationData.frameTime, animationData.loop, animationData.reverse);
	} else {
		assert(0 && "Error: sprite not found!");
		return Animation(nullptr, 0, 0);
	}
}

Animation::Animation() {}

Animation::Animation(const SpriteInfo* spriteInfo, float frameTime, int loop, bool reverse) : m_spriteInfo(spriteInfo), m_frameTime(frameTime), m_loop(loop), m_startLoop(loop) {
	m_step = reverse ? -1 : 1;
	restart();
}

void Animation::update(float dt) {
	if (m_loop == 0) {
		return;
	}

	m_elapsedTime += dt;

	if (m_elapsedTime >= m_frameTime) {
		m_elapsedTime = 0.f;

		m_currentFrame = (m_currentFrame + m_step) % m_spriteInfo->num_frames;
	}

	if (m_currentFrame == m_lastFrame) {
		--m_loop;
	}
}

void Animation::setLoop(int loop) {
	m_loop = loop;
}

bool Animation::isOver() const {
	return m_loop == 0;
}

void Animation::restart() {
	m_elapsedTime = 0.f;
	if (m_step == -1) {
		m_currentFrame = m_spriteInfo->num_frames - 1;
		m_lastFrame = 0;
	} else {
		m_currentFrame = 0;
		m_lastFrame = m_spriteInfo->num_frames - 1;
	}
	m_startFrame = m_currentFrame;
	m_loop = m_startLoop;
}

const SpriteInfo* Animation::getSpriteInfo() const {
	return m_spriteInfo;
}
