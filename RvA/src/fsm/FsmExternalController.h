#pragma once

#include "fsm/FsmTypes.h"

namespace flow {
class FsmExternalController {
public:
	virtual ~FsmExternalController() = default;
	virtual FsmAction update(const HashId& currentStateName) = 0;
};

} // namespace flow
