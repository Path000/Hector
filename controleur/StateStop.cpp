#include "StateStop.h"

void StateStop::setStateIdle(StateIdle* stateIdle) {
	_stateIdle = stateIdle;
}

void StateStop::onStart() {
	_robot->_ecran.clear();
	_robot->_ecran.set(0, "Stop");
	_robot->_ecran.refresh();

  	_robot->stopMotors();
}

State* StateStop::run() {
	if (duration() > DISPLAY_STOP_DELAY) {
		return((State*)_stateIdle);
	}
	return NULL;
}

