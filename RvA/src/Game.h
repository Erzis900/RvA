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

class Game
{
public:
	Game();
	~Game();

	void setState(std::unique_ptr<IGameState> newState);
	void run();

	auto& getGUI() { return m_gui; }
	auto& getAtlas() { return m_atlas; }

	const auto& getDefenderRegistry() const { return m_defenderTypeRegistry; }
	const auto& getBulletTypeRegistry() const { return m_bulletTypeRegistry; }
	const auto& getEnemyTypeRegistry() const { return m_enemyTypeRegistry; }

	MusicManager& getMusicManager() { return m_musicManager; }

	void scheduleClose();

private:
	std::unique_ptr<IGameState> m_currentState;
	std::unique_ptr<IGameState> m_nextState;

	void update();
	void updateRenderRec();
	void updateMouse();
	void updateTransition(float dt);
	bool shouldClose() const;

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

	float m_fadeAlpha{};
	float m_transitionSpeed{};
	bool m_fadingOut{};
	bool m_fadingIn{};
	bool m_scheduleClose{};

	GUI m_gui;
	Atlas m_atlas;
	DefenderTypeRegistry m_defenderTypeRegistry;
	BulletTypeRegistry m_bulletTypeRegistry;
	EnemyTypeRegistry m_enemyTypeRegistry;
	MusicManager m_musicManager;
};
