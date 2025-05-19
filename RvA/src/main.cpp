#define RAYGUI_IMPLEMENTATION

#define NOMINMAX

#include "Game.h"

#include <raygui.h>
#include <raylib_win32.h>

int main() {
	SetTraceLogLevel(LOG_ERROR);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Game game;
	game.run();

	return 0;
}
