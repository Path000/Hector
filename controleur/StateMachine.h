#ifndef StateMachine_h
#define StateMachine_h

#include "State.h"
#include "Robot.h"
#include "Command.h"
#include "StateIdle.h"
#include "StateWelcome.h"
#include "StateChoc.h"
#include "Compteur.h"

class StateMachine {
	public:
		void init(Robot* robot);
		void changeState(State* state);
		void run();
	private:
		Robot* _robot;
		State* _currentState;
		StateIdle _stateIdle;
		StateChoc _stateChoc;
		StateWelcome _stateWelcome;
};


#endif
