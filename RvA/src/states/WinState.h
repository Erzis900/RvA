#pragma once

#include "fsm/FsmState.h"
#include "IGameState.h"
#include "Animation.h"
#include "utilities/CallbackRegistry.h"

#include <vector>
#include <memory>

class WinState : public IGameState, public flow::FsmState
{
public:
	WinState(Game& game);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	struct SpriteItem {
		const SpriteInfo* spriteInfo;
		Vector2 position;
		Vector2 velocity;
		std::unique_ptr<Animation> animation;
		Flip flip{};
	};

	SpriteItem createSpriteItem(const SpriteInfo* spriteInfo, const Vector2& position, const Vector2& velocity);
	void updateSprites(std::vector<SpriteItem>& sprites, float dt);
	void drawSprites(std::vector<SpriteItem>& sprites);
	void drawBullets(std::vector<SpriteItem>& sprites);

	std::vector<SpriteItem> m_defenders;
	std::vector<SpriteItem> m_chasers;
	std::vector<SpriteItem> m_bullets;

	float m_textAnimationTime{};
	float m_textAnimationSpeed{2.5f};
	float m_textAnimationAmplitude{10.f};
	Vector2 m_textPosition;
	Game& m_game;
    CallbackHandle m_drawingCallbackHandle;
};
