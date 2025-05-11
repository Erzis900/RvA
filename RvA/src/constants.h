#pragma once

#include <raylib.h>
#include <tuple>

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

std::tuple<int, int> getCoordinates(const Vector2& position);
Vector2 getSnappedPosition(const Vector2& position);
Vector2 getSnappedPosition(int row, int column);
