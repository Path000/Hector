#ifndef StateStop_h
#define StateStop_h

#include "State.h"
#include "StateIdle.h"

#define DISPLAY_STOP_DELAY 1000

class StateStop : public State {
	public:
		void onStart();
		State* run();
		void setStateIdle(StateIdle* stateIdle);
	private:
		StateIdle* _stateIdle;
		
};




#endif
