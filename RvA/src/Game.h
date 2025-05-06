#pragma once

#include "GUI/GUI.h"
#include "IGameState.h"
#include "MusicManager.h"
#include "Session.h"
#include "atlas/Atlas.h"
#include "bullet/BulletManager.h"
#include "collisions/CollisionSystem.h"
#include "defender/DefenderManager.h"
#include "enemy/EnemyManager.h"
#include "fsm/Fsm.h"
#include "utilities/CallbackRegistry.h"

#include <memory>
#include <raylib.h>

class Game {
public:
	Game();
	~Game();

	void run();

	auto& getGUI() {
		return m_gui;
	}

	auto& getAtlas() {
		return m_atlas;
	}

	const auto& getDefenderRegistry() const {
		return m_defenderTypeRegistry;
	}

	const auto& getBulletTypeRegistry() const {
		return m_bulletTypeRegistry;
	}

	const auto& getEnemyTypeRegistry() const {
		return m_enemyTypeRegistry;
	}

	auto& getMusicManager() {
		return m_musicManager;
	}

	auto& getGameSession() {
		return m_gameSession;
	}

	// When it comes to rendering we should move to a more retained approach
	// Instead of propagating calls by calling Class::draw the idea is to move out from that pattern
	// and as a temporary solution we can use a callback system instead.
	// If someone is interested in rendering something custom they can register a callback
	[[nodiscard]] CallbackHandle registerDrawingCallback(std::function<void()> callback) {
		return m_drawCallbacks.registerCallback(std::move(callback));
	}

private:
	std::unique_ptr<IGameState> m_currentState;
	std::unique_ptr<IGameState> m_nextState;

	void update();
	void updateRenderRec();
	void updateMouse();
	bool shouldClose() const;

	void setupFSM();

	void draw();

	void registerDefenderTypes();
	void registerBulletTypes();
	void registerEnemyTypes();

	RenderTexture2D m_renderTexture;
	Rectangle m_renderRec;

	float m_scale{};

	int m_texWidth{};
	int m_texHeight{};

	int m_screenWidth{};
	int m_screenHeight{};

	bool m_isTransitionInProgress{};

	GUI m_gui;
	Atlas m_atlas;
	MusicManager m_musicManager;

	DefenderTypeRegistry m_defenderTypeRegistry;
	BulletTypeRegistry m_bulletTypeRegistry;
	EnemyTypeRegistry m_enemyTypeRegistry;

	Session m_gameSession;
	std::unique_ptr<flow::Fsm> m_fsm;
	CallbackRegistry<> m_drawCallbacks;
};
