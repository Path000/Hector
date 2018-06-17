#include "StateChoc.h"

void StateChoc::setStateIdle(StateIdle* stateIdle) {
	_stateIdle = stateIdle;
}

void StateChoc::onStart() {
	_robot->getEcran()->clear();
	for(int line=0; line<4; line++) _robot->getEcran()->set(line, "!! Choc !!");
	_robot->getEcran()->refresh();

  	_robot->emergencyStop();
		_robot->getCommand()->send("CHOC");
}

State* StateChoc::run() {
	if (duration() > CHOC_DELAY) {
		return((State*)_stateIdle);
	}
	return NULL;
}

void StateChoc::onStop() {
}
