#include "StateStop.h"

void StateStop::setStateIdle(StateIdle* stateIdle) {
	_stateIdle = stateIdle;
}

void StateStop::onStart() {
	_robot->getEcran()->clear();
	for(int line=0; line<4; line++) _robot->getEcran()->set(line, "STOP");
	_robot->getEcran()->refresh();

	_robot->stopMoteurs();

	// TODO wait counters à 0
}

State* StateStop::run() {
	if (duration() > STOP_DELAY) {
		return((State*)_stateIdle);
	}
	return NULL;
}

void StateStop::onStop() {
	_robot->getEcran()->clear();
}
