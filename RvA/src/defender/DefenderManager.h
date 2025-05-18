#pragma once

#include "Animation.h"
#include "GameAction.h"
#include "collisions/Collider.h"
#include "defender/DefenderTypeRegistry.h"
#include "utilities/CallbackRegistry.h"
#include "utilities/Task.h"

#include <array>
#include <constants.h>
#include <memory>
#include <vector>

class CollisionSystem;
class MusicManager;

/*
 * The Defender struct contains almost only data related to a single instance
 */
struct Defender {
	const DefenderTypeInfo* info{};
	Vector2 position{};
	Animation animation;
	float shootTime{};
	float prepareShootTime{};
	bool hasFired{};
	float scrapsGainTime{};
	int hp{};
	int row{};
	int column{};
	ColliderHandle colliderHandle{};
	DefenderState state{DefenderState::On};
	Color tint{WHITE};
	TaskContainer task{};
};

struct DefenderUpdateResult {
	float amountOfBatteryDrain{};
	GameActions actions;
};

class DefenderManager {
public:
	DefenderManager(CollisionSystem& collisionSystem, MusicManager& musicManager);

	void clear();
	void draw(Atlas& atlas);
	DefenderUpdateResult update(float dt);

	const std::vector<std::unique_ptr<Defender>>& getDefenders() const;

	void toggleDefender(int row, int column);
	void enableDefender(Defender& defender);
	void disableDefender(Defender& defender);
	Defender& spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column);
	void setState(Defender& defender, DefenderState state);
	bool hasDefender(int row, int column) const;

	CallbackHandle onDefenderDestroyed(std::function<void(Defender&)> callback);

private:
	void performDying(Defender& defender);

	std::vector<std::unique_ptr<Defender>> m_defenders;
	std::array<std::array<Defender*, COLS>, ROWS> m_defenderGrid = {nullptr};
	CallbackRegistry<Defender&> m_onDefenderDestroyedCallbacks;
	CollisionSystem& m_collisionSystem;
	MusicManager& m_musicManager;
	DefenderUpdateResult m_updateResult;
};
