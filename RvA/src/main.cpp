#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include <raylib_win32.h>
#include <spdlog/spdlog.h>

#include "Game.h"

int main()
{
	SetTraceLogLevel(LOG_ERROR);
	spdlog::info("Hello from spdlog");

	Game game;
	game.run();

	return 0;
}