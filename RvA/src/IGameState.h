#pragma once

class Game;

class IGameState
{
public:
	virtual ~IGameState() = default;
	virtual void update(Game& game) {};
	virtual void draw(Game& game) = 0;
};