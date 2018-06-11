#include "StateWelcome.h"

void StateWelcome::setStateIdle(StateIdle* stateIdle) {
	_stateIdle = stateIdle;
}

void StateWelcome::onStart() {
	_robot->getEcran()->set(0, "Hector");
	_robot->getEcran()->set(1, " Hector");
	_robot->getEcran()->set(2, "  Hector");
	_robot->getEcran()->set(3, "   Hector");
	_robot->getEcran()->refresh();
}

State* StateWelcome::run() {	
	if (duration() > WELCOME_DELAY) {
		return((State*)_stateIdle);
	}
	return NULL;
}

void StateWelcome::onStop() {
	_robot->getEcran()->clear();
}
