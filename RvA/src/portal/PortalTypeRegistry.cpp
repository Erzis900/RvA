#include "PortalTypeRegistry.h"

void PortalTypeRegistry::registerPortal(std::string id, PortalTypeInfo PortalTypeInfo) {
	m_portalTypes.insert({std::move(id), std::move(PortalTypeInfo)});
}

const PortalTypeInfo* PortalTypeRegistry::getPortalInfo(const std::string& id) const {
	auto itr = m_portalTypes.find(id);
	return (itr != m_portalTypes.end()) ? &itr->second : nullptr;
}
