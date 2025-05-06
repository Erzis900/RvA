#pragma once

#include "BulletTypes.h"

#include <string>
#include <unordered_map>

class BulletTypeRegistry {
public:
	void registerBulletType(std::string id, BulletData typeInfo);
	const BulletData* getBulletInfo(const std::string& id) const;

private:
	std::unordered_map<std::string, BulletData> m_bulletTypes;
};
