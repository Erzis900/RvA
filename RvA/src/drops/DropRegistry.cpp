#include "DropRegistry.h"

void DropRegistry::addDrop(DropType type, DropTypeInfo info) {
	m_dropTypes[type] = info;
}

const DropTypeInfo* DropRegistry::getDropType(DropType type) const {
	auto it = m_dropTypes.find(type);
	if (it != m_dropTypes.end()) {
		return &it->second;
	}
	return nullptr;
}
