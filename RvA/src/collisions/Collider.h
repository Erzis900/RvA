#pragma once

#include <cassert>
#include <raylib.h>
#include <tuple>
#include <unordered_set>
#include <variant>

using ColliderHandle = int;

struct Bullet;
struct Defender;
struct Wall;
class Enemy;

using ColliderOwner = std::variant<::Bullet*, ::Enemy*, ::Defender*, ::Wall*>;

struct Collider {
	enum Flag {
		Defender = 1 << 0,
		Bullet = 1 << 1,
		Enemy = 1 << 2,
		BaseWall = 1 << 3
	};

	ColliderHandle handle;
	Flag flag;
	Rectangle boundingBox;
	ColliderOwner owner;
	std::unordered_set<ColliderHandle> collidingWith;
};

consteval int cMask(Collider::Flag flagA, Collider::Flag flagB) {
	return flagA | flagB;
}

enum class CollisionEvent {
	Enter,
	Ongoing,
	Exit
};

struct Collision {
	int mask;
	CollisionEvent event;
	Collider* colliderA{};
	Collider* colliderB{};

	bool check(Collider::Flag flagA, Collider::Flag flagB) const {
		return mask == (flagA | flagB);
	}

	template<typename Ta, typename Tb> std::tuple<Ta*, Tb*> extractOwners() const {
		Ta* ptr1{};
		Tb* ptr2{};

		if (auto ptr = std::get_if<Ta*>(&colliderA->owner)) {
			ptr1 = *ptr;
		} else if (auto ptr = std::get_if<Tb*>(&colliderA->owner)) {
			ptr2 = *ptr;
		}
		if (auto ptr = std::get_if<Ta*>(&colliderB->owner)) {
			ptr1 = *ptr;
		} else if (auto ptr = std::get_if<Tb*>(&colliderB->owner)) {
			ptr2 = *ptr;
		}

		assert(ptr1 != nullptr || ptr2 != nullptr);
		if constexpr (std::is_same_v<Ta, Tb>) {
			assert(ptr1 != ptr2);
		}
		return std::make_tuple(ptr1, ptr2);
	}
};
