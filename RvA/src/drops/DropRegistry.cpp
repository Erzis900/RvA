#include "DropRegistry.h"

void DropRegistry::addDrop(std::string id, DropTypeInfo info) {
	m_dropTypes[std::move(id)] = std::move(info);
}

const DropTypeInfo* DropRegistry::getDropType(const std::string& id) const {
	auto it = m_dropTypes.find(id);
	if (it != m_dropTypes.end()) {
		return &it->second;
	}
	return nullptr;
}
