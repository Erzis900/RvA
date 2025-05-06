#define RAYGUI_IMPLEMENTATION

#define NOMINMAX

#include "Game.h"

#include <raygui.h>
#include <raylib_win32.h>
#include <spdlog/spdlog.h>

int main() {
	SetTraceLogLevel(LOG_ERROR);
	spdlog::info("Hello from spdlog");

	Game game;
	game.run();

	return 0;
}
