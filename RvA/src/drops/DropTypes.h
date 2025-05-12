#pragma once

#include <Animation.h>
#include <optional>

enum class DropType {
	Scraps,
	Battery,
};

struct DropTypeInfo {
	DropType type{};
	Animation idleAnimation{};
	float animationDuration{};
	std::optional<Vector2> size{};
};

struct Drop {
	const DropTypeInfo* info{};
	int amount{};
	Vector2 position{};
	Vector2 size{};
	Vector2 startPosition{};
	float enterAnimation{};
};
