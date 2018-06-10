#include "StateMachine.h"

void StateMachine::init(Robot* robot) {
	_robot = robot;

  _stateIdle.init(_robot);
  _stateChoc.init(_robot);
  _stateChoc.setStateIdle(&_stateIdle);
  _stateWelcome.init(_robot);
  _stateWelcome.setStateIdle(&_stateIdle);
  _stateMove.init(_robot);
  _stateStop.init(_robot);
  _stateStop.setStateIdle(&_stateIdle);

	changeState((State*)&_stateWelcome);
}

void StateMachine::changeState(State* newState) {
	if(newState == NULL) return;

	if(_currentState) _currentState->onStop();

	_currentState = newState;
	_currentState->initStartedAt();
	_currentState->onStart();
}

void StateMachine::run() {

	if (_robot->getCommand()->read()) {
		ParsedCommand command = _robot->getCommand()->get();
		if(command.cmd == "MOVE") {

			_stateMove.setStrafe(command.arrayArgs[0].toInt());
			_stateMove.setRotation(command.arrayArgs[1].toInt());
			
			changeState(&_stateMove);
		}
		if(command.cmd == "STOP") {
			changeState(&_stateStop);
		}
	}

  if(_robot->getSensor()->detectImpact()) {
    changeState(&_stateChoc);
  }
  
	changeState(_currentState->run());
}
