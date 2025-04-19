#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#undef RAYGUI_IMPLEMENTATION

#include "Game.h"
#include "states/MenuState.h"
#include <memory>

int main()
{
	Game game;
	game.run();

	return 0;
}