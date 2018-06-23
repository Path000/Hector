#ifndef StateMove_h
#define StateMove_h

#include "State.h"
#include "StateIdle.h"

#include "Compteur.h"

#define UPDATE_PID_PERIOD 100

class StateMove : public State {
	public:
		void onStart();
		State* run();
		void setData(String strafeDirection, String rotationDirection);
	private:
		unsigned long _lastRefresh;
};


#endif