#pragma once

#include "IGameState.h"

#include <memory>
#include <raylib.h>

#include "GUI/GUI.h"
#include "atlas/Atlas.h"
#include "MusicManager.h"

#include "defender/DefenderManager.h"
#include "bullet/BulletManager.h"
#include "enemy/EnemyManager.h"

#include "collisions/CollisionSystem.h"
#include "Session.h"

class Game
{
public:
	Game();
	~Game();

	template<typename T, bool UseFade = true, typename ...Args>
    void setState(Args&&... args) requires std::derived_from<T, IGameState> {
        static_assert(std::is_constructible_v<T, Args&&...>, "T must be constructible with Args...");
        internalSetState(std::make_unique<T>(std::forward<Args>(args)...), UseFade);
    }

	void internalSetState(std::unique_ptr<IGameState> newState, bool useFade);
	void run();

	auto& getGUI() { return m_gui; }
	auto& getAtlas() { return m_atlas; }

	const auto& getDefenderRegistry() const { return m_defenderTypeRegistry; }
	const auto& getBulletTypeRegistry() const { return m_bulletTypeRegistry; }
	const auto& getEnemyTypeRegistry() const { return m_enemyTypeRegistry; }
	auto& getMusicManager() { return m_musicManager; }
    auto& getGameSession() { return m_gameSession; }

	void scheduleClose();

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
	bool m_scheduleClose{};

	GUI m_gui;
	Atlas m_atlas;
	MusicManager m_musicManager;

	DefenderTypeRegistry m_defenderTypeRegistry;
	BulletTypeRegistry m_bulletTypeRegistry;
	EnemyTypeRegistry m_enemyTypeRegistry;

	Session m_gameSession;
};
