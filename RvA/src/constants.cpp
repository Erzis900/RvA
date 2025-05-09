#include "constants.h"

#include <cmath>

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
