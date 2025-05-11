#pragma once

#include "Config.h"
#include "GUI/GUI.h"
#include "GameRegistry.h"
#include "MusicManager.h"
#include "Session.h"
#include "atlas/Atlas.h"
#include "fsm/Fsm.h"
#include "utilities/CallbackRegistry.h"
#include "utilities/Random.h"

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

	const auto& getGameRegistry() const {
		return m_gameRegistry;
	}

	auto& getMusicManager() {
		return m_musicManager;
	}

	auto& getGameSession() {
		return m_gameSession;
	}

	auto& getConfig() {
		return m_config;
	}

	void setRenderTextureColor(Color color) {
		m_renderTextureColor = color;
	}

	// When it comes to rendering we should move to a more retained approach
	// Instead of propagating calls by calling Class::draw the idea is to move out from that pattern
	// and as a temporary solution we can use a callback system instead.
	// If someone is interested in rendering something custom they can register a callback
	[[nodiscard]] CallbackHandle registerDrawingCallback(std::function<void()> callback) {
		return m_drawCallbacks.registerCallback(std::move(callback));
	}

private:
	void update();
	void updateRenderRec();
	void updateMouse();
	bool shouldClose() const;

	void setupFSM();

	void draw();

	void registerDefenderTypes();
	void registerBulletTypes();
	void registerEnemyTypes();
	void registerDropTypes();
	void registerLevels();
	void verifyLevelData();
	void verifyLevelCoordinate(int min, int max, const ConfigValue<int>& value);

	RenderTexture2D m_renderTexture;
	Rectangle m_renderRec;
	Color m_renderTextureColor{BLACK};

	float m_scale{};

	int m_texWidth{};
	int m_texHeight{};

	int m_screenWidth{};
	int m_screenHeight{};

	bool m_isTransitionInProgress{};

	GUI m_gui;
	Atlas m_atlas;
	Config m_config;
	MusicManager m_musicManager;

	GameRegistry m_gameRegistry;

	Session m_gameSession;
	std::unique_ptr<flow::Fsm> m_fsm;
	CallbackRegistry<> m_drawCallbacks;
	Random m_random;
};
