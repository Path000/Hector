#ifndef StateIdle_h
#define StateIdle_h

#include <Arduino.h>
#include "State.h"
#include "Robot.h"

#define LINE_CHANGE_DELAY 500

class StateIdle : public State {
	public:
		void onStart();
		State* run();
		void onStop();
	private:
		byte line;
		unsigned long lastLineChange;
		void writeLine();
};

#endif