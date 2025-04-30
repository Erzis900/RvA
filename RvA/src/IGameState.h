#pragma once

class Game;

class IGameState
{
public:
	virtual ~IGameState() = default;
	virtual void update(Game& game, float dt) {};
	virtual void draw(Game& game) = 0;

	virtual void onEnter(Game& game) {};
	virtual void onExit(Game& game) {};
};