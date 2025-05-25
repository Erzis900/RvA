#pragma once

#include <raylib.h>
#include <string>
#include <tuple>
#include <vector>

#ifdef __EMSCRIPTEN__
#define WEB_MODE
#else
#define DESKTOP_MODE
#endif

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

constexpr Vector2 GAME_RENDERTEXTURE_SIZE = {640, 360};
constexpr Vector2 UI_RENDERTEXTURE_SIZE = {640, 360};

constexpr int ROWS = 7;
constexpr int COLS = 18;

constexpr int CELL_SIZE = 32;
constexpr Vector2 GRID_OFFSET = {64, 64};

constexpr bool DEV_MODE = true;

constexpr int FONT_SMALL = 16;
constexpr int FONT_MEDIUM = 32;
constexpr int FONT_BIG = 64;

constexpr Color DEFAULT_BKG_COLOR = {30, 46, 73, 255};

constexpr bool FORCE_LEVEL_SEQUENCE = false;
extern std::vector<std::string> FORCED_LEVEL_SEQUENCE;

std::tuple<int, int> getCoordinates(const Vector2& position);
Vector2 getSnappedPosition(const Vector2& position);
Vector2 getSnappedPosition(int row, int column);
int clampColumn(int column);
int clampRow(int row);

extern Vector2 virtualMousePosition;
Vector2 getCorrectedMousePosition();
