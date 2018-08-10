#ifndef StateMachine_h
#define StateMachine_h

#define STARTING_DELAY 10000
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

		StateError stateError;
		StateStartingUp stateStartingUp;
		StateScanning stateScanning;
		StateComputing stateComputing;

	private:
		State* _currentState;
};

/* =================================================================================
 * static variable declared in main INO file
 * =================================================================================*/

#include "Ecran.h"
#include "RPLidar.h"
extern RPLidar lidar;
extern Ecran ecran;
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
	ecran.clear();
	ecran.set(0, "HECTOR");
	ecran.set(1, "Bonjour !!");
	ecran.set(2, "");
	ecran.set(3, "");
	ecran.refresh();

	// Start rotating and waits for speed stability
	lidar.startMotor();
}

void StateStartingUp::run() {
	if (duration() > STARTING_DELAY) {
		stateMachine.changeState((State*)&stateMachine.stateScanning);
	}
}


/* =================================================================================
 * STATE SCANNING
 * =================================================================================*/

void StateScanning::onStart() {

	ecran.clear();
	ecran.set(0, "SCANNING");
	ecran.refresh();

	lidar.startScan();

	if(lidar.status.code == RPL_STATUS_ERROR) {
		stateMachine.stateError.setMessage(lidar.status.message);
		stateMachine.changeState((State*) &stateMachine.stateError);
	}
}

void StateScanning::run() {

	lidar.loopScan();

	switch(lidar.status.code) {

		case RPL_STATUS_ERROR :

			lidar.stopScanning();
			lidar.stopMotor();
			stateMachine.stateError.setMessage(lidar.status.message);
			stateMachine.changeState((State*) &stateMachine.stateError);
			break;

		case RPL_STATUS_STOP :

			lidar.stopScanning();
			stateMachine.changeState((State*) &stateMachine.stateComputing);
			break;

		case RPL_STATUS_CONTINUE :
		default :
			break;
	}
}


/* =================================================================================
 * STATE COMPUTING
 * =================================================================================*/

void StateComputing::onStart() {

	ecran.clear();

	for(uint16_t i = 0; i < lidar.scanIndexTop; i++) {
		ecran.pixel(map(lidar.scans[i].x, -8000, 8000, 0, 63)+64, map(lidar.scans[i].y, -8000, 8000, 0, 63));
	}
}

void StateComputing::run() {

	if (duration() > COMPUTING_DELAY) {

		stateMachine.changeState((State*) &stateMachine.stateScanning);
	}
}



/* =================================================================================
 * STATE ERROR (END OF LIFE)
 * =================================================================================*/

void StateError::onStart() {

	_line = 0;
	_lastLineChange = millis();
}

void StateError::setMessage(const char* message) {
	_message = message;
}

void StateError::writeLine() {
	ecran.clear();
	ecran.set(_line, _message);
	ecran.refresh();
}

void StateError::run() {
	if(millis() - _lastLineChange > LINE_CHANGE_DELAY) {
		writeLine();
		_line = (_line+1)%8;
		_lastLineChange = millis();
	}
}




/* =================================================================================
 * StateMachine
 * =================================================================================*/

void StateMachine::init() {

	changeState((State*) &stateStartingUp);
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