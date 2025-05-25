#pragma once

#include "utilities/CallbackRegistry.h"

#include <memory>

namespace flow {
class Fsm;
}

template<typename T> class ConfigValue;

class Atlas;
class Config;
class GameRegistry;
class GUI;
class MusicManager;
class Session;
class ResourceSystem;

class Game {
public:
	Game();
	~Game();

	void run();

	GUI& getGUI();
	Atlas& getAtlas();
	const GameRegistry& getGameRegistry() const;
	MusicManager& getMusicManager();
	Session& getGameSession();
	Config& getConfig();
	ResourceSystem& getResourceSystem();

	// When it comes to rendering we should move to a more retained approach
	// Instead of propagating calls by calling Class::draw the idea is to move out from that pattern
	// and as a temporary solution we can use a callback system instead.
	// If someone is interested in rendering something custom they can register a callback
	[[nodiscard]] CallbackHandle registerDrawingCallback(std::function<void()> callback);

private:
	void setupFSM();
	void save();

	void registerDefenderTypes();
	void registerBulletTypes();
	void registerEnemyTypes();
	void registerDropTypes();
	void registerPortals();

	static void updateWeb();

	/*
	 * Pimpl idiom to avoid recompilation of every class depending on Game.h
	 */
	struct pimpl;
	std::unique_ptr<pimpl> m_pimpl;
};
