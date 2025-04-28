#include "DefenderManager.h"
#include "Game.h"
#include <iostream>
#include "enemy/enemyManager.h"
#include "bullet/ShooterBullet.h"

DefenderManager::DefenderManager(Game& game)
	: m_game(game)
{
}

void DefenderManager::update(float dt, int cellSize, int rows, float& energy, int &batteries, EnemyManager& enemyManager)
{
    for (auto& defender : m_defenders)
    {
        defender->update(dt, energy, batteries);
    }

    for (auto& bullet : m_bullets)
    {
		if (!bullet->isActive()) continue;

        bullet->update(dt, m_game);

		for (auto& enemy : enemyManager.getEnemies())
		{
            if (ShooterBullet* shooterBullet = dynamic_cast<ShooterBullet*>(bullet.get()))
            {
                if (enemy->getRow() == shooterBullet->getRow())
                {
                    if (bullet->getPosition().x > enemy->getPosition().x)
                    {
                        bullet->setActive(false);
                        enemy->takeDamage(50);
                    }
                }
            }
		}
    }

    m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(),
        [](const std::unique_ptr<Bullet>& b) { return !b->isActive(); }),
        m_bullets.end());

    handlePlace(cellSize, rows);
}

void DefenderManager::draw(int cellSize)
{
    for (auto& defender : m_defenders)
    {
        defender->draw(m_game, cellSize);
    }

    for (auto& bullet : m_bullets)
    {
        bullet->draw(m_game);
    }
}

void DefenderManager::spawnBullet(std::unique_ptr<Bullet> bullet)
{
    m_bullets.push_back(std::move(bullet));
}

void DefenderManager::handlePlace(int cellSize, int rows)
{
    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
    {
        Vector2 mousePos = GetMousePosition();

        if (mousePos.x > cellSize && mousePos.y > cellSize && mousePos.x < m_game.getTexSize().x - cellSize &&
            mousePos.y < m_game.getTexSize().y - (m_game.getTexSize().y - (rows + 1) * cellSize))
        {
            int row = int(mousePos.x) / cellSize - 1;
            int col = int(mousePos.y) / cellSize - 1;

            DefenderType type = m_game.getGUI().getSelectedDefender();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && type != DefenderType::None)
            {
                if (!m_occupied[row][col])
                {
                    float x = float(row * cellSize + cellSize);
                    float y = float(col * cellSize + cellSize);

                    int cost = m_costs[static_cast<int>(type)];

                    if (cost <= m_game.getGUI().getBatteries())
                    {
                        m_defenders.push_back(std::make_unique<Defender>(Vector2{ x, y }, row, col, cost, type, m_game, *this));
                        m_occupied[row][col] = true;
						m_game.getGUI().getBatteries() -= cost;
                    }
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