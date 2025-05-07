#pragma once

#include "Animation.h"
#include "collisions/Collider.h"
#include "defender/DefenderTypeRegistry.h"
#include "defender/DefenderTypes.h"
#include "utilities/CallbackRegistry.h"

#include <array>
#include <constants.h>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

class CollisionSystem;

enum class DefenderState {
	On,
	Off,
	PrepareToShoot,
	ReadyToShoot,
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

struct BulletSpawnAction {
	std::string bulletType;
	Vector2 position;
};

using Actions = std::vector<std::variant<BulletSpawnAction>>;

struct DefenderUpdateResult {
	float amountOfBatteryDrain{};
	float amountOfScrapsGain{};
	Actions actions;
};

class DefenderManager {
public:
	DefenderManager(CollisionSystem& collisionSystem);

	void clear();
	void draw(Atlas& atlas);
	DefenderUpdateResult update(float dt);

	const std::vector<std::unique_ptr<Defender>>& getDefenders() const;

	void toggleDefender(int row, int column);
	void spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column);
	bool hasDefender(int row, int column) const;
	void setState(Defender& defender, DefenderState state);

	CallbackHandle onDefenderDestroyed(std::function<void(int, int)> callback);

private:
	void performPrepareShoot(Defender& defender, float dt);
	void performDying(Defender& defender);

	std::vector<std::unique_ptr<Defender>> m_defenders;
	std::array<std::array<Defender*, COLS>, ROWS> m_defenderGrid = {nullptr};
	CallbackRegistry<int, int> m_onDefenderDestroyedCallbacks;
	CollisionSystem& m_collisionSystem;
};
