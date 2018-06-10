#ifndef StateStop_h
#define StateStop_h

#include "State.h"
#include "StateIdle.h"

#define STOP_DELAY 500

class StateStop : public State {
	public:
		void onStart();
		State* run();
		void onStop();
		void setStateIdle(StateIdle* stateIdle);
	private:
		StateIdle* _stateIdle;
};


#endif