#ifndef StateChoc_h
#define StateChoc_h

#include "State.h"
#include "StateIdle.h"

#define CHOC_DELAY 3000

class StateChoc : public State {
	public:
		void onStart();
		State* run();
		void setStateIdle(StateIdle* stateIdle);
	private:
		StateIdle* _stateIdle;
};


#endif