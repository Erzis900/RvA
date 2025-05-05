#pragma once

#include <raylib.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

constexpr int TEX_WIDTH = 640;
constexpr int TEX_HEIGHT = 360;

constexpr int ROWS = 8;
constexpr int COLS = 18;

constexpr int CELL_SIZE = 32;
constexpr float MAX_BATTERY_CHARGE = 100.f;
constexpr Vector2 GRID_OFFSET = { CELL_SIZE, CELL_SIZE * 2.f };

constexpr bool DEV_MODE = true;

constexpr int FONT_SMALL = 16;
constexpr int FONT_MEDIUM = 32;
constexpr int FONT_BIG = 64;