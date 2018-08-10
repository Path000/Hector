#ifndef StateError_h
#define StateError_h

#define LINE_CHANGE_DELAY 1000

#include "State.h"

class StateError : public State {
	public:
		State* onStart();
		State* run();
		void setMessage(const char* message);
	private:
		const char* _message;
		byte _line;
		unsigned long _lastLineChange;
		void writeLine();

};

#endif