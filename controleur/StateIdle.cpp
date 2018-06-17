#include "StateIdle.h"

void StateIdle::onStart() {
	_robot->getEcran()->clear();
	line = 0;
	lastLineChange = millis();
	writeLine();
}

void StateIdle::writeLine() {
	_robot->getEcran()->clear();
	_robot->getEcran()->set(line, "Idle");
	_robot->getEcran()->refresh();
}

State* StateIdle::run() {
	if(millis() - lastLineChange > LINE_CHANGE_DELAY) {
		line = (line+1)%4;
		writeLine();
		lastLineChange = millis();
		_robot->getCommand()->send("READY");
	}
	return NULL;
}

void StateIdle::onStop() {
}
