#include "StateMachine.h"

void StateMachine::init(Robot* robot) {
	_robot = robot;

  _stateIdle.init(_robot);
  _stateChoc.init(_robot);
  _stateChoc.setStateIdle(&_stateIdle);
  _stateWelcome.init(_robot);
  _stateWelcome.setStateIdle(&_stateIdle);
  _stateMove.init(_robot);

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

	ParsedCommandType* command = _robot->_command.readIfAvailable();

	if (command->newCommandAvailable) {

		Serial.print(command->cmd);
		Serial.print(":");
		Serial.print(command->arrayArgs[0]);
		Serial.print(":");
		Serial.println(command->arrayArgs[1]);

		if(command->cmd == "MOVE") {

			_stateMove.setData(command->arrayArgs[0], command->arrayArgs[1]);

			changeState(&_stateMove);
		}
	}

  if(_robot->_sensor.detectImpact()) {

    changeState(&_stateChoc);
  }

	changeState(_currentState->run());
}
