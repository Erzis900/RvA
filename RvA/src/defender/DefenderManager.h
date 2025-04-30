#pragma once
#include <vector>
#include <memory>
#include <array>
#include <variant>
#include <optional>
#include <unordered_map>
#include "Defender.h"
#include <constants.h>
#include "bullet/Bullet.h"
#include "bullet/BulletTypes.h"

class Game;
class EnemyManager;

class DefenderManager
{
public:
    DefenderManager(Game& game);

    void update(float dt, float& energy, float &batteries, EnemyManager& enemyManager);
    void draw();

    void spawnBullet(std::unique_ptr<Bullet> bullet);

    const std::array<int, static_cast<int>(DefenderType::None)>& getCosts() const
    {
        return m_costs;
    }

private:
    Game& m_game;
    std::vector<std::unique_ptr<Defender>> m_defenders;
	std::array<std::array<bool, COLS>, ROWS> m_occupied = { false };
    std::array<int, static_cast<int>(DefenderType::None)> m_costs = { 0, 10, 20, 30 };
    std::vector<std::unique_ptr<Bullet>> m_bullets;
    
    void handlePlace(float& batteries);
};

class GUI;

/*
* The DefenderTypeInfo gives us information about the type of defender
* For example what's the max number of HP or how much it cost, and so on.
*/
struct DefenderTypeInfo
{
    DefenderType type{};
    std::string spriteEnabled;
    std::string spriteDisabled;
    float energyDrain{};
    float batteryGain{};
    float firstShootCooldown{};
    float shootCooldown{};
    int maxHP{};
    int cost{};
    std::optional<BulletType> bulletType;
};

/*
* The Defender struct contains almost only data related to a single instance
*/
struct Defender2
{
    const DefenderTypeInfo* info{};
    Vector2 position;
    bool isActive{};
    Animation animation;
    float shootTime{};
    float batteryGainTime{};
    int hp{};
    int row{};
    int column{};
};

struct BulletSpawnAction
{
    BulletType bulletType;
    Vector2 position;
};

using Actions = std::vector<std::variant<BulletSpawnAction>>;

struct DefenderUpdateResult
{
    float amountOfEnergyDrain{};
    float amountOfBatteryGain{};
    Actions actions;
};

class DefenderTypeRegistry
{
public:
    void registerDefender(DefenderTypeInfo defenderTypeInfo);
    const DefenderTypeInfo* getDefenderInfo(DefenderType type) const;

private:
    std::unordered_map<DefenderType, DefenderTypeInfo> m_defenderTypes;
};

class DefenderManager2
{
public:
    DefenderManager2(Atlas& atlas, GUI& gui);

    void draw();
    DefenderUpdateResult update(float dt);

    const std::vector<std::unique_ptr<Defender2>>& getDefenders() const;

    void toggleDefender(int row, int column);
    void spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column);
    bool hasDefender(int row, int column) const;

private:
    std::vector<std::unique_ptr<Defender2>> m_defenders;
    std::array<std::array<Defender2*, COLS>, ROWS> m_defenderGrid = { nullptr };
    Atlas& m_atlas;
    GUI& m_gui;
};
