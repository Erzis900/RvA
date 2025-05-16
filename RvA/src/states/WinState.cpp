#include "WinState.h"

#include "GUI/GUI.h"
#include "Game.h"
#include "GameRegistry.h"
#include "constants.h"

#include <raylib.h>
#include <raymath.h>

WinState::WinState(Game& game) : m_game(game) {}

flow::FsmAction WinState::enter() {
	m_defenders.clear();
	auto solarPanelInfo = m_game.getGameRegistry().getDefender("Solarpanel");
	m_defenders.push_back(createSpriteItem(solarPanelInfo->spriteEnabled.spriteInfo, {GAME_RENDERTEXTURE_SIZE.x, 80}, {-150, 0}));

	m_chasers.clear();
	auto x = GAME_RENDERTEXTURE_SIZE.x + 100.f;
	for (auto i = 0; i < 3; ++i) {
		const auto typeInfo = m_game.getGameRegistry().getEnemy("B1");
		m_chasers.push_back(createSpriteItem(typeInfo->moveAnimation.spriteInfo, {x, 80}, {-150, 0}));
		x += 30.f;
	}

	m_bullets.clear();
	for (auto i = 0; i < 4; ++i) {
		m_bullets.push_back(createSpriteItem(nullptr, {-100, 96}, {}));
	}

	// clang-format off
	auto btnSize = Vector2{100, 30.f};
	auto& gui = m_game.getGUI();
	gui.buildScreen("Win")
		.default_bkg(0.5f)
		.shape({ .pos = { 0, 100 }, .size = { autoSize, 20 }, .color = Fade(WHITE, 0.5f), .type = ShapeType::Rectangle })
		.stack({ .orientation = GUIOrientation::Vertical, .padding = {0, 5}, .hAlign = HAlign::Center, .vAlign = VAlign::Center, .sideAlignContent = ContentAlign::Center})
			.medium_text({.text = "Level Completed!!!", .color = WHITE, .hAlign = HAlign::Center})
			.space({0, 35.f})
			.button({"Next Level", {}, btnSize, [this]() { goToNextLevel(); }})
		.end()
		.custom({
			.pos = { 0, 10 },
			.draw = [this](Atlas& atlas, const auto& rect) {		
				drawSprites(atlas, m_defenders);
				drawSprites(atlas, m_chasers);
				drawBullets(atlas, m_bullets);
			},
			.hAlign = HAlign::Center,
            .vAlign = VAlign::Bottom
		});
	// clang-format on

	return flow::FsmAction::none();
}

void WinState::exit() {
	m_drawingCallbackHandle = {};
	m_game.getGUI().destroyScreen("Win");
}

flow::FsmAction WinState::update(float dt) {
	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

	updateSprites(m_defenders, dt);
	updateSprites(m_chasers, dt);
	updateSprites(m_bullets, dt);

	// When the last chaser reach x position -100 we let them run away and start spawning the bullets
	if (m_chasers.back().position.x <= -100) {
		auto x = -20.f;
		for (auto& sprite : m_chasers) {
			sprite.velocity.x *= -1;
			sprite.position.x = x;
			sprite.flip = Flip::Horizontal;
			x -= 30;
		}

		x -= 50;
		for (auto& sprite : m_bullets) {
			sprite.velocity.x = 160;
			sprite.position.x = x;
			x -= 25;
		}
	}

	m_textAnimationTime += dt;
	m_textPosition.y = sinf(m_textAnimationTime * m_textAnimationSpeed) * m_textAnimationAmplitude;

	return flow::FsmAction::none();
}

WinState::SpriteItem WinState::createSpriteItem(const SpriteInfo* spriteInfo, const Vector2& position, const Vector2& velocity) {
	return {spriteInfo, position, velocity, spriteInfo ? std::make_unique<Animation>(Animation::createAnimation({spriteInfo, 0.05f})) : nullptr};
}

void WinState::updateSprites(std::vector<SpriteItem>& sprites, float dt) {
	for (auto& sprite : sprites) {
		auto offset = Vector2Scale(sprite.velocity, dt);
		sprite.position += offset;

		if (sprite.animation) {
			sprite.animation->update(dt);
		}
	}
}

void WinState::drawSprites(Atlas& atlas, std::vector<SpriteItem>& sprites) {
	for (auto& sprite : sprites) {
		if (sprite.animation) {
			m_game.getAtlas().drawSprite(sprite.spriteInfo, sprite.position, sprite.animation->getCurrentFrame(), sprite.flip);
		}
	}
}

void WinState::drawBullets(Atlas& atlas, std::vector<SpriteItem>& sprites) {
	for (auto& sprite : sprites) {
		DrawCircleV(Vector2Add(sprite.position, {-2, -2}), 2.5f, Fade(BLACK, 0.5f));
		DrawCircleV(sprite.position, 2.5f, WHITE);
	}
}

void WinState::goToNextLevel() {
	m_game.getGUI().startFadingInOut([this] { m_nextTransition = "next"; }, [this] {}, 0.5f);
}

