#pragma once

class Game;
class GUI;

class IGameState
{
public:
	virtual ~IGameState() = default;
	virtual void update(Game& game) {};
	virtual void draw(Game& game, GUI& gui) = 0;
};