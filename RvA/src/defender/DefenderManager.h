#pragma once

#include <vector>
#include <memory>
#include <array>
#include <variant>
#include <optional>
#include <unordered_map>
#include <constants.h>

#include "Animation.h"
#include "defender/DefenderTypes.h"
#include "collisions/Collider.h"

class CollisionSystem;

enum class DefenderState
{
    On,
    Off,
    PrepareToShoot,
    ReadyToShoot,
    Dying,
    Dead
};

/*
* The DefenderTypeInfo gives us information about the type of defender
* For example what's the max number of HP or how much it cost, and so on.
*/
struct DefenderTypeInfo
{
    DefenderType type{};
    AnimationData spriteEnabled;
    AnimationData spriteDisabled;
    AnimationData spriteShoot;
    float batteryDrain{};
    float scrapsGain{};
    float firstShootCooldown{};
    // TODO: one of the cooldowns is not used, remove one of them
	float shootCooldown{}; // this is not used anywhere, why is it here?
    int maxHP{};
    int cost{};
    std::optional<std::string> bulletType;
    float shootingAnimationTime{};
};

/*
* The Defender struct contains almost only data related to a single instance
*/
struct Defender
{
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
	DefenderState state{ DefenderState::On };
    Color tint{ WHITE };
};

struct BulletSpawnAction
{
    std::string bulletType;
    Vector2 position;
};

using Actions = std::vector<std::variant<BulletSpawnAction>>;

struct DefenderUpdateResult
{
    float amountOfBatteryDrain{};
    float amountOfScrapsGain{};
    Actions actions;
};

class DefenderTypeRegistry
{
public:
    void registerDefender(DefenderTypeInfo defenderTypeInfo);
    const DefenderTypeInfo* getDefenderInfo(DefenderType type) const;
    auto& getDefenderInfos() const { return m_defenderTypes; }

private:
    std::unordered_map<DefenderType, DefenderTypeInfo> m_defenderTypes;
};

class DefenderManager
{
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

private:
    void performPrepareShoot(Defender& defender, float dt);

    std::vector<std::unique_ptr<Defender>> m_defenders;
    std::array<std::array<Defender*, COLS>, ROWS> m_defenderGrid = { nullptr };
    CollisionSystem& m_collisionSystem;
};
