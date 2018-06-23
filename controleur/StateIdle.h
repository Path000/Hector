#ifndef StateIdle_h
#define StateIdle_h

#include <Arduino.h>
#include "State.h"
#include "Robot.h"

#define LINE_CHANGE_DELAY 1000

class StateIdle : public State {
	public:
		void onStart();
		State* run();
	private:
		byte line;
		unsigned long lastLineChange;
		void writeLine();
};

#endif