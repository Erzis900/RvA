#pragma once

#include "fsm/FsmAction.h"

/**
 * @brief Implement this abstract class to create a new State that can be added to an Fsm.
 * FsmState derives from FsmEventHandler, enabling reaction to platform application events (such as input, window
 * events, etc.) by overriding the appropriate functions.
 */
namespace flow {

class FsmState {
public:
	virtual ~FsmState() = default;

	/*
	 * @brief Called during each Fsm::update() iteration if this state is the current state in the Fsm.
	 */
	virtual FsmAction update(float dt) = 0;

	/**
	 * @brief Invoked within Fsm::update() when transitioning to this state as the new current state.
	 */
	virtual FsmAction enter() = 0;

	/**
	 * @brief Invoked within Fsm::update() when this state is replaced by a new current state.
	 */
	virtual void exit() = 0;
};

} // namespace flow
