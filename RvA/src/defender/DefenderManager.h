#pragma once

#include "Animation.h"
#include "GameAction.h"
#include "collisions/Collider.h"
#include "defender/DefenderTypeRegistry.h"
#include "utilities/CallbackRegistry.h"

#include <array>
#include <constants.h>
#include <memory>
#include <vector>

class CollisionSystem;

enum class DefenderState {
	On,
	Off,
	Shooting,
	Dying,
	Dead
};

/*
 * The Defender struct contains almost only data related to a single instance
 */
struct Defender {
	const DefenderTypeInfo* info{};
	Vector2 position{};
	Animation animation;
	float shootTime{};
	float prepareShootTime{};
	float scrapsGainTime{};
	int hp{};
	int row{};
	int column{};
	ColliderHandle colliderHandle{};
	DefenderState state{DefenderState::On};
	Color tint{WHITE};
};

struct DefenderUpdateResult {
	float amountOfBatteryDrain{};
	float amountOfScrapsGain{};
	GameActions actions;
};

class DefenderManager {
public:
	DefenderManager(CollisionSystem& collisionSystem);

	void clear();
	void draw(Atlas& atlas);
	DefenderUpdateResult update(float dt);

	const std::vector<std::unique_ptr<Defender>>& getDefenders() const;

	void toggleDefender(int row, int column);
	Defender* getDefender(int row, int column);
	Defender& spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column);
	bool hasDefender(int row, int column) const;
	void setState(Defender& defender, DefenderState state);

	CallbackHandle onDefenderDestroyed(std::function<void(Defender&)> callback);

private:
	void performDying(Defender& defender);

	std::vector<std::unique_ptr<Defender>> m_defenders;
	std::array<std::array<Defender*, COLS>, ROWS> m_defenderGrid = {nullptr};
	CallbackRegistry<Defender&> m_onDefenderDestroyedCallbacks;
	CollisionSystem& m_collisionSystem;
};
