#ifndef StateStartingUp_h
#define StateStartingUp_h

#include "StateScanning.h"

#define WELCOME_DELAY 10000

class StateStartingUp : public State {
	public:
		State* onStart();
		State* run();
		void setStateScanning(StateScanning* stateScanning);
	private:
		StateScanning* _stateScanning;
};

#endif