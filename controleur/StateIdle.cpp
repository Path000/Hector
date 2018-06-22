#include "StateIdle.h"

void StateIdle::onStart() {
	_robot->_ecran.clear();
	line = 0;
	lastLineChange = millis();
	writeLine();
}

void StateIdle::writeLine() {
	_robot->_ecran.clear();
	_robot->_ecran.set(line, "Idle");
	_robot->_ecran.refresh();
}

State* StateIdle::run() {
	if(millis() - lastLineChange > LINE_CHANGE_DELAY) {
		line = (line+1)%4;
		writeLine();
		lastLineChange = millis();
		_robot->_command.send("READY");
	}
	return NULL;
}

void StateIdle::onStop() {
}
