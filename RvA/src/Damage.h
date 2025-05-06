#pragma once

enum class DamageSource {
	Bullet,
	BaseWall,
};

struct Damage {
	float value{};
	float bounceBackPower{};
	DamageSource source{DamageSource::Bullet};
};

struct DamageInfo {
	float baseDamage{};
	float bounceBackPower{};
	bool dealDamageOverTime{}; // If true: multiply the damage value by the deltaTime

	Damage createDamage(float dt) {
		return {.value = dealDamageOverTime ? baseDamage * dt : baseDamage, .bounceBackPower = bounceBackPower};
	}
};
