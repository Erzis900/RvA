#pragma once

#include "DefenderTypes.h"

class Session;
class DefenderTypeRegistry;

class DefenderPicker {
public:
	DefenderPicker(Session& session);

	void update(float dt);

	bool canAfford(DefenderType type) const;
	bool isInCooldown(DefenderType type) const;
	void startCooldown(DefenderType type);

private:
	Session& m_gameSession;
	const DefenderTypeRegistry& m_defenderTypeRegistry;
};
