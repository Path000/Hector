#include "StateError.h"

State* StateError::onStart() {

	_robot->_lidar.stopMotor();
	_robot->_lidar.stopScanning();

	_line = 0;
	_lastLineChange = millis();
	writeLine();
}

void StateError::setMessage(const char* message) {
	_message = message;
}

void StateError::writeLine() {
	_robot->_ecran.clear();
	_robot->_ecran.set(_line, _message);
	_robot->_ecran.refresh();
}

State* StateError::run() {
	if(millis() - _lastLineChange > LINE_CHANGE_DELAY) {
		_line = (_line+1)%8;
		writeLine();
		_lastLineChange = millis();
	}
	return NULL;
}
