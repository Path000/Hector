#include "StateMachine.h"

void StateMachine::init(Robot* robot) {
	_robot = robot;

  _stateIdle.init(_robot);
  _stateChoc.init(_robot);
  _stateChoc.setStateIdle(&_stateIdle);
  _stateWelcome.init(_robot);
  _stateWelcome.setStateIdle(&_stateIdle);

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

	ParsedCommandType* command = _robot->getCommand()->readIfAvailable();
	if (command->newCommandAvailable) {
		if(command->cmd == "MOVE") {
			_robot->computeMove(command->arrayArgs[0], command->arrayArgs[1]);
		}
	}

	SpeedSampleType* compteur1 = _robot->getCompteur1()->readIfAvailable();

	if (compteur1->newSampleAvailable) {

    _robot->getMoteurA()->setCurrentSpeed(compteur1->speed2);
    _robot->getMoteurB()->setCurrentSpeed(compteur1->speed1);

    _robot->getEcran()->set(2, String("A:")+String(compteur1->speed2)+String(" B:")+String(compteur1->speed1));
    _robot->getEcran()->refresh();
  }

  SpeedSampleType* compteur2 = _robot->getCompteur2()->readIfAvailable();
  
  if (compteur2->newSampleAvailable) {

    _robot->getMoteurC()->setCurrentSpeed(compteur2->speed1);
    _robot->getMoteurD()->setCurrentSpeed(compteur2->speed2);

		_robot->getEcran()->set(3, String("D:")+String(compteur2->speed2)+String(" C:")+String(compteur2->speed1));
		_robot->getEcran()->refresh();
  }

  if(_robot->getSensor()->detectImpact()) {
  	// Stop motors
  	_robot->computeMove(-1, 0);
    changeState(&_stateChoc);
  }

  if(_robot->getMoteurA()->update() +
  	_robot->getMoteurB()->update() +
  	_robot->getMoteurC()->update() + 
  	_robot->getMoteurD()->update() == 0) {
  	changeState(&_stateIdle);
  }

  
	changeState(_currentState->run());
}
