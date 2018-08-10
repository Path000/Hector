#ifndef StateMachine_h
#define StateMachine_h

#include "Robot.h"
#include "State.h"
#include "StateError.h"
#include "StateStartingUp.h"
#include "StateScanning.h"

class StateMachine {
	public:
		void init(Robot* robot);
		void changeState(State* state);
		void run();
	private:
		Robot* _robot;
		State* _currentState;

		StateError _stateError;
		StateStartingUp _stateStartingUp;
		StateScanning _stateScanning;
};

#endif