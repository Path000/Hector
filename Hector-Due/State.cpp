#include "State.h"

void State::init(Robot* robot) {
	_robot = robot;
}

void State::initStartedAt() {
	_startedAt = millis();
}

unsigned long State::duration() {
	return (millis() - _startedAt);
}

