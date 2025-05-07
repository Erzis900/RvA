#pragma once

#include <Animation.h>

enum class DropType {
	Scraps,
	Battery,
};

struct DropTypeInfo {
	DropType type{};
	Animation idleAnimation{};
};

struct Drop {
	const DropTypeInfo* info{};
	int amount{};
	Vector2 position{};
	Vector2 startPosition{};
	Vector2 endPosition{};
	float enterAnimation{};
};
