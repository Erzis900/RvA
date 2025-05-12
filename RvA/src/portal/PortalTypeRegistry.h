#pragma once

#include "Animation.h"
#include "PortalTypes.h"

#include <unordered_map>

struct PortalTypeInfo {
	PortalType type{};
	AnimationData spriteIdle;
	AnimationData spriteClose;
	float lifespan{};
};

class PortalTypeRegistry {
public:
	void registerPortal(std::string id, PortalTypeInfo portalTypeInfo);
	const PortalTypeInfo* getPortalInfo(const std::string& id) const;

	auto& getPortalInfos() const {
		return m_portalTypes;
	}

private:
	std::unordered_map<std::string, PortalTypeInfo> m_portalTypes;
};
