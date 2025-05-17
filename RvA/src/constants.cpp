#include "constants.h"

#include <algorithm>
#include <cmath>

std::vector<std::string> FORCED_LEVEL_SEQUENCE = {
	"raylibDemo",
};

std::tuple<int, int> getCoordinates(const Vector2& position) {
	auto row = static_cast<int>(std::floor((position.y - GRID_OFFSET.y) / CELL_SIZE));
	auto column = static_cast<int>(std::floor((position.x - GRID_OFFSET.x) / CELL_SIZE));
	return {row, column};
}

Vector2 getSnappedPosition(const Vector2& position) {
	auto [row, column] = getCoordinates(position);
	return {GRID_OFFSET.x + column * CELL_SIZE, GRID_OFFSET.y + row * CELL_SIZE};
}

Vector2 getSnappedPosition(int row, int column) {
	return {GRID_OFFSET.x + column * CELL_SIZE, GRID_OFFSET.y + row * CELL_SIZE};
}

int clampColumn(int column) {
	return std::clamp(column, 0, COLS - 1);
}

int clampRow(int row) {
	return std::clamp(row, 0, ROWS - 1);
}
