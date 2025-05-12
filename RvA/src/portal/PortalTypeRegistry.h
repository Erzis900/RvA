#pragma once

#include "Animation.h"
#include "PortalTypes.h"

#include <unordered_map>

struct PortalTypeInfo {
	PortalType type{};
	AnimationData spriteIdle;
	AnimationData spriteClose;
	float lifespan;
};

class PortalTypeRegistry {
public:
	void registerPortal(PortalTypeInfo portalTypeInfo);
	const PortalTypeInfo* getPortalInfo(PortalType type) const;

	auto& getPortalInfos() const {
		return m_portalTypes;
	}

private:
	std::unordered_map<PortalType, PortalTypeInfo> m_portalTypes;
};
