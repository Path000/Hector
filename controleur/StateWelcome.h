#ifndef StateWelcome_h
#define StateWelcome_h

#include "State.h"
#include "StateIdle.h"

#define WELCOME_DELAY 1000

class StateWelcome : public State {
	public:
		void onStart();
		State* run();
		void onStop();
		void setStateIdle(StateIdle* stateIdle);
	private:
		StateIdle* _stateIdle;
		
};




#endif
