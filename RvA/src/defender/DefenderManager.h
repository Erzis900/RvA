#pragma once
#include <vector>
#include <memory>
#include <array>
#include "Defender.h"
#include <constants.h>

class Game;

class DefenderManager
{
public:
    DefenderManager(Game& game);

    void update(float dt, int cellSize, int rows, float& energy, int &batteries);
    void draw(int cellSize);

    const std::array<int, static_cast<int>(DefenderType::None)>& getCosts() const
    {
        return m_costs;
    }

private:
    Game& m_game;
    std::vector<std::unique_ptr<Defender>> m_defenders;
	std::array<std::array<bool, ROWS>, COLS> m_occupied = { false };
    std::array<int, static_cast<int>(DefenderType::None)> m_costs = { 0, 10, 20, 30 };

    void handlePlace(int cellSize, int rows);
};