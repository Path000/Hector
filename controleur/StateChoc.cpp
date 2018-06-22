#include "StateChoc.h"

void StateChoc::setStateIdle(StateIdle* stateIdle) {
	_stateIdle = stateIdle;
}

void StateChoc::onStart() {
	_robot->_ecran.clear();
	for(int line=0; line<4; line++) _robot->_ecran.set(line, "!! Choc !!");
	_robot->_ecran.refresh();

  	_robot->emergencyStop();
		_robot->_command.send("CHOC");
}

State* StateChoc::run() {
	if (duration() > CHOC_DELAY) {
		return((State*)_stateIdle);
	}
	return NULL;
}

void StateChoc::onStop() {
}
