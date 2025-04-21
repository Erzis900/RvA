#include "MenuState.h"
#include "Game.h"

MenuState::MenuState(Game& game)
{}

void MenuState::draw(Game& game)
{
	game.getGUI().drawMenu();
}