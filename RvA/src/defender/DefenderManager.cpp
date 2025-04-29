#include "DefenderManager.h"
#include "Game.h"
#include <iostream>
#include "enemy/enemyManager.h"
#include "bullet/ShooterBullet.h"

DefenderManager::DefenderManager(Game& game)
	: m_game(game)
{
}

void DefenderManager::update(float dt, float& energy, int &batteries, EnemyManager& enemyManager)
{
    for (auto& defender : m_defenders)
    {
        defender->update(dt, energy, batteries);

        for (auto& enemy : enemyManager.getEnemies())
        {
			if (defender->getRow() == enemy->getRow())
			{
                if (enemy->getPosition().x <= defender->getPosition().x + CELL_SIZE && enemy->getPosition().x > defender->getPosition().x)
                {
                    enemy->setTargetDefender(defender.get());
                }   
			}
        }
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

    m_defenders.erase(
        std::remove_if(m_defenders.begin(), m_defenders.end(),
            [](const std::unique_ptr<Defender>& defender)
            {
                return defender->getHp() <= 0.f;

            }),
        m_defenders.end()
    );

    handlePlace();
}

void DefenderManager::draw()
{
    for (auto& defender : m_defenders)
    {
        defender->draw(m_game);
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

void DefenderManager::handlePlace()
{
    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
    {
        Vector2 mousePos = GetMousePosition();

        if (mousePos.x > CELL_SIZE && mousePos.y > CELL_SIZE && mousePos.x < TEX_WIDTH - CELL_SIZE &&
            mousePos.y < TEX_HEIGHT - (TEX_HEIGHT - (ROWS + 1) * CELL_SIZE))
        {
            int row = int(mousePos.y) / CELL_SIZE - 1;
            int col = int(mousePos.x) / CELL_SIZE - 1;

            DefenderType type = m_game.getGUI().getSelectedDefender();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && type != DefenderType::None)
            {
                if (!m_occupied[row][col])
                {
                    float x = float(col * CELL_SIZE + CELL_SIZE);
                    float y = float(row * CELL_SIZE + CELL_SIZE);

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