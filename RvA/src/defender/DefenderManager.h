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

    void update(float dt, int cellSize, int rows, float& energy);
    void draw(int cellSize);

private:
    Game& m_game;
    std::vector<std::unique_ptr<Defender>> m_defenders;
	std::array<std::array<bool, ROWS>, COLS> m_occupied = { false };

    void handlePlace(int cellSize, int rows);
};