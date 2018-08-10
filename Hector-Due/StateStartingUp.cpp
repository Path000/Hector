#include "StateStartingUp.h"

void StateStartingUp::setStateScanning(StateScanning* stateScanning) {
	_stateScanning = stateScanning;
}

State* StateStartingUp::onStart() {
	_robot->_ecran.clear();
	_robot->_ecran.set(0, "Hector");
	_robot->_ecran.set(1, "Starting up");
	_robot->_ecran.set(2, "");
	_robot->_ecran.set(3, "");
	_robot->_ecran.refresh();

	_robot->_lidar.startMotor();
}

State* StateStartingUp::run() {
	if (duration() > WELCOME_DELAY) {
		return((State*)_stateScanning);
	}
	return NULL;
}

