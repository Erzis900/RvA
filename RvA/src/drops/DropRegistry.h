#pragma once

#include "DropTypes.h"

#include <unordered_map>

class DropRegistry {
public:
	void addDrop(std::string id, DropTypeInfo info);
	const DropTypeInfo* getDropType(const std::string& id) const;

private:
	std::unordered_map<std::string, DropTypeInfo> m_dropTypes;
};
