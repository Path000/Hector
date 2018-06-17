#ifndef StateMove_h
#define StateMove_h

#include "State.h"
#include "StateIdle.h"

#include "Compteur.h"

#define REFRESH_DELAY 3000

class StateMove : public State {
	public:
		void onStart();
		State* run();
		void onStop();
		void setData(int strafeDirection, byte rotationDirection);
	private:
		int _strafeDirection;
		byte _rotationDirection;
		unsigned long _lastRefresh;
};


#endif