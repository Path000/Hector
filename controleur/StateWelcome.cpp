#include "StateWelcome.h"

void StateWelcome::setStateIdle(StateIdle* stateIdle) {
	_stateIdle = stateIdle;
}

void StateWelcome::onStart() {
	_robot->_ecran.clear();
	_robot->_ecran.set(0, "Hector");
	_robot->_ecran.set(1, " Hector");
	_robot->_ecran.set(2, "  Hector");
	_robot->_ecran.set(3, "   Hector");
	_robot->_ecran.refresh();
}

State* StateWelcome::run() {	
	if (duration() > WELCOME_DELAY) {
		return((State*)_stateIdle);
	}
	return NULL;
}

void StateWelcome::onStop() {
}
