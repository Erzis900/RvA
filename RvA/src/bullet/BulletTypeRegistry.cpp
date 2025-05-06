#include "BulletTypeRegistry.h"

void BulletTypeRegistry::registerBulletType(std::string id, BulletData data) {
	m_bulletTypes.insert({std::move(id), std::move(data)});
}

const BulletData* BulletTypeRegistry::getBulletInfo(const std::string& id) const {
	if (auto itr = m_bulletTypes.find(id); itr != m_bulletTypes.end()) {
		return &itr->second;
	}

	return nullptr;
}
