#pragma once
#include <vector>
#include <memory>
#include <array>
#include "Defender.h"
#include <constants.h>
#include "bullet/Bullet.h"

class Game;
class EnemyManager;

class DefenderManager
{
public:
    DefenderManager(Game& game);

    void update(float dt, float& energy, int &batteries, EnemyManager& enemyManager);
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
    
    void handlePlace();
};