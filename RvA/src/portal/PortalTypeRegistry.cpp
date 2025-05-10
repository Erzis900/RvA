#include "PortalTypeRegistry.h"

void PortalTypeRegistry::registerPortal(PortalTypeInfo PortalTypeInfo) {
	m_portalTypes.insert({PortalTypeInfo.type, std::move(PortalTypeInfo)});
}

const PortalTypeInfo* PortalTypeRegistry::getPortalInfo(PortalType type) const {
	auto itr = m_portalTypes.find(type);
	return (itr != m_portalTypes.end()) ? &itr->second : nullptr;
}
