#include "StateMachine.h"

void StateMachine::init(Robot* robot) {

	_robot = robot;

	_stateStartingUp.init(_robot);
	_stateStartingUp.setStateScanning(&_stateScanning);

	_stateScanning.init(_robot);
	_stateScanning.setStateError(&_stateError);
  
	_stateError.init(_robot);

	changeState((State*)&_stateStartingUp);
}

void StateMachine::changeState(State* newState) {

	_currentState = newState;
	_currentState->initStartedAt();
	State* newNewState = _currentState->onStart();
	if(newNewState != NULL && newNewState != _currentState) changeState(newNewState);
}

void StateMachine::run() {

	State* newState = _currentState->run();

	if(newState != NULL) changeState(newState);
}