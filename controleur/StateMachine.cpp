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

	_currentState = newState;
	_currentState->initStartedAt();
	_currentState->onStart();
}

void StateMachine::run() {

	ParsedCommandType* command = _robot->_command.readIfAvailable();

	if (command->newCommandAvailable) {

		if(command->cmd == "MOVE") {

			_stateMove.setData(command->arrayArgs[0], command->arrayArgs[1]);

			changeState(&_stateMove);
		}

		if(command->cmd == "STOP") {

			changeState(&_stateStop);
		}
	}

  if(_robot->_sensor.detectImpact()) {

    changeState(&_stateChoc);
  }

	changeState(_currentState->run());
}
