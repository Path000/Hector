#include "StateScanning.h"

void StateScanning::setStateError(StateError* stateError) {
	_stateError = stateError;
}

State* StateScanning::onStart() {

	const char* errorMessage = _robot->_lidar.startScan();

	if(errorMessage) {
		_stateError->setMessage(errorMessage);
		return _stateError;
	}
	return NULL;
}

State* StateScanning::run() {

	const char* errorMessage = _robot->_lidar.loopScan();

	if(errorMessage) {
		_stateError->setMessage(errorMessage);
		return _stateError;
	}

	if(_robot->_lidar.revolution) {
		return _stateComputing;
	}

	return NULL;
}

