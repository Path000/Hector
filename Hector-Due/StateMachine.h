#ifndef StateMachine_h
#define StateMachine_h

#define WELCOME_DELAY 10000
#define COMPUTING_DELAY 3000
#define LINE_CHANGE_DELAY 1000

#include <Arduino.h>

class State {
	public:
		void initStartedAt();
		unsigned long duration();
		virtual void onStart();
		virtual void run(); // Returns new state ptr or NULL if no state change
	private:
		unsigned long _startedAt;
};

class StateStartingUp : public State {
	public:
		void onStart();
		void run();
};

class StateScanning : public State {
	public:
		void onStart();
		void run();
};

class StateComputing : public State {
	public:
		void onStart();
		void run();
};

class StateError : public State {
	public:
		void onStart();
		void run();
		void setMessage(const char* message);
	private:
		const char* _message;
		byte _line;
		unsigned long _lastLineChange;
		void writeLine();

};

class StateMachine {
	public:
		void init();
		void changeState(State* state);
		void run();

		StateError _stateError;
		StateStartingUp _stateStartingUp;
		StateScanning _stateScanning;
		StateComputing _stateComputing;

	private:
		State* _currentState;
};

/* =================================================================================
 * static variable declared in main INO file
 * =================================================================================*/

#include "Robot.h"
extern Robot robot;
extern StateMachine stateMachine;

/* =================================================================================
 * STATE SUPER CLASS
 * =================================================================================*/

void State::initStartedAt() {
	_startedAt = millis();
}

unsigned long State::duration() {
	return (millis() - _startedAt);
}


/* =================================================================================
 * STATE STARTING UP
 * =================================================================================*/

void StateStartingUp::onStart() {
	robot._ecran.clear();
	robot._ecran.set(0, "HECTOR");
	robot._ecran.set(1, "Bonjour !!");
	robot._ecran.set(2, "");
	robot._ecran.set(3, "");
	robot._ecran.refresh();

	robot._lidar.startMotor();
}

void StateStartingUp::run() {
	if (duration() > WELCOME_DELAY) {
		stateMachine.changeState((State*)&stateMachine._stateScanning);
	}
}


/* =================================================================================
 * STATE SCANNING
 * =================================================================================*/

void StateScanning::onStart() {

	robot._ecran.clear();
	robot._ecran.set(0, "SCANNING");
	robot._ecran.refresh();

	const char* errorMessage = robot._lidar.startScan();

	if(errorMessage) {
		stateMachine._stateError.setMessage(errorMessage);
		stateMachine.changeState((State*)&stateMachine._stateError);
	}
}

void StateScanning::run() {

	const char* errorMessage = robot._lidar.loopScan();

	if(errorMessage) {
		stateMachine._stateError.setMessage(errorMessage);
		stateMachine.changeState((State*)&stateMachine._stateError);
		return;
	}

	if(robot._lidar.revolution) {
		stateMachine.changeState((State*)&stateMachine._stateComputing);
	}
}


/* =================================================================================
 * STATE COMPUTING
 * =================================================================================*/

void StateComputing::onStart() {

	robot._ecran.clear();
	robot._ecran.set(0, "COMPUTING");
	robot._ecran.refresh();
}

void StateComputing::run() {

	if (duration() > COMPUTING_DELAY) {

		stateMachine.changeState((State*)&stateMachine._stateScanning);
	}
}



/* =================================================================================
 * STATE ERROR (END OF LIFE)
 * =================================================================================*/

void StateError::onStart() {

	robot._lidar.stopMotor();
	robot._lidar.stopScanning();

	_line = 0;
	_lastLineChange = millis();
	writeLine();
}

void StateError::setMessage(const char* message) {
	_message = message;
}

void StateError::writeLine() {
	robot._ecran.clear();
	robot._ecran.set(_line, _message);
	robot._ecran.refresh();
}

void StateError::run() {
	if(millis() - _lastLineChange > LINE_CHANGE_DELAY) {
		_line = (_line+1)%8;
		writeLine();
		_lastLineChange = millis();
	}
}




/* =================================================================================
 * StateMachine
 * =================================================================================*/

void StateMachine::init() {

	changeState((State*)&_stateStartingUp);
}

void StateMachine::changeState(State* newState) {

	_currentState = newState;
	_currentState->initStartedAt();
	_currentState->onStart();
}

void StateMachine::run() {

	_currentState->run();
}

#endif