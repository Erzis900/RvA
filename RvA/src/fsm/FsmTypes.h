#pragma once

#include "fsm/FsmState.h"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace flow {

struct FsmTransitionTarget {
	HashId stateName;
	std::function<bool()> condition{};
};

using FsmTransitions = std::unordered_map<HashId, std::vector<FsmTransitionTarget>>;

struct FsmStateEntry {
	HashId name;
	std::unique_ptr<FsmState> state;
	FsmTransitions transitions;
};

using FsmStateEntries = std::unordered_map<HashId, std::unique_ptr<FsmStateEntry>>;

struct FsmStateInfo {
	HashId name;
	FsmTransitions transitions;
};

struct FsmInfo {
	std::vector<FsmStateInfo> states;
	HashId startState;
};

} // namespace flow
