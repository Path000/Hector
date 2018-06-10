#ifndef State_h
#define State_h

#include "Robot.h"

class State {
	public:
		void init(Robot* robot);
		void initStartedAt();
		unsigned long duration();
		virtual void onStart();
		virtual State* run(); // Returns new state ptr or NULL if no state change
		virtual void onStop();
	protected:
		Robot* _robot;
	private:
		unsigned long _startedAt;
};

#endif