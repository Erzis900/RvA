#pragma once

#include <Animation.h>

enum class DropType {
	Scraps,
	Battery,
};

struct DropTypeInfo {
	DropType type{};
	Animation spritInfo{};
};

struct Drop {
	const DropTypeInfo* info{};
	int amount{};
	Vector2 position{};
	float enterAnimation{};
};
