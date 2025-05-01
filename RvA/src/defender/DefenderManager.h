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

class GUI;

/*
* The DefenderTypeInfo gives us information about the type of defender
* For example what's the max number of HP or how much it cost, and so on.
*/
struct DefenderTypeInfo
{
    DefenderType type{};
    AnimationData spriteEnabled;
    AnimationData spriteDisabled;
    float batteryDrain{};
    float scrapsGain{};
    float firstShootCooldown{};
    float shootCooldown{};
    int maxHP{};
    int cost{};
    std::optional<std::string> bulletType;
};

/*
* The Defender struct contains almost only data related to a single instance
*/
struct Defender
{
    const DefenderTypeInfo* info{};
    Vector2 position{};
    bool isActive{};
    Animation animation;
    float shootTime{};
    float scrapsGainTime{};
    int hp{};
    int row{};
    int column{};
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
    DefenderManager(Atlas& atlas, GUI& gui);

    void draw();
    DefenderUpdateResult update(float dt);

    const std::vector<std::unique_ptr<Defender>>& getDefenders() const;

    void toggleDefender(int row, int column);
    void spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column);
    bool hasDefender(int row, int column) const;

private:
    std::vector<std::unique_ptr<Defender>> m_defenders;
    std::array<std::array<Defender*, COLS>, ROWS> m_defenderGrid = { nullptr };
    Atlas& m_atlas;
    GUI& m_gui;
};
