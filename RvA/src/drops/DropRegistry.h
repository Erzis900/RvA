#pragma once

#include "DropTypes.h"

#include <unordered_map>

class DropRegistry {
public:
	void addDrop(DropType type, DropTypeInfo info);
	const DropTypeInfo* getDropType(DropType type) const;

private:
	std::unordered_map<DropType, DropTypeInfo> m_dropTypes;
};
