#include "DefenderManager.h"
#include "Game.h"
#include <iostream>

DefenderManager::DefenderManager(Game& game)
	: m_game(game)
{
}

void DefenderManager::update(float dt, int cellSize, int rows, float& energy)
{
    for (auto& defender : m_defenders)
    {
        defender->update(dt, energy);
    }
    handlePlace(cellSize, rows);
}

void DefenderManager::draw(int cellSize)
{
    for (auto& defender : m_defenders)
    {
        defender->draw(m_game, cellSize);
    }
}

void DefenderManager::handlePlace(int cellSize, int rows)
{
	DefenderType type = m_game.getGUI().getSelectedDefender();

    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
    {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x > cellSize && mousePos.y > cellSize && mousePos.x < m_game.getTexSize().x - cellSize &&
            mousePos.y < m_game.getTexSize().y - (m_game.getTexSize().y - (rows + 1) * cellSize))
        {
            int row = int(mousePos.x) / cellSize;
            int col = int(mousePos.y) / cellSize;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && type != DefenderType::None)
            {
                if (!m_occupied[row][col])
                {
                    float x = float(row * cellSize);
                    float y = float(col * cellSize);

                    m_defenders.push_back(std::make_unique<Defender>(Vector2{ x, y }, row, col, type, m_game.getAtlas()));
                    m_occupied[row][col] = true;
				}
			}
			else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
			{
                if (m_occupied[row][col])
                {
                    for (auto& defender : m_defenders)
                    {
                        if (defender->getRow() == row && defender->getCol() == col)
                        {
                            defender->setActive(!defender->isActive());
                            break;
                        }
                    }
                }
            }
        }
    }
}