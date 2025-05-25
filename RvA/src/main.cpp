#define RAYGUI_IMPLEMENTATION

#define NOMINMAX

#include "Game.h"

#include <raygui.h>

#ifdef _WIN32
#include <raylib_win32.h>
#endif

int main() {
	SetTraceLogLevel(LOG_ERROR);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Game game;
	game.run();

	return 0;
}
