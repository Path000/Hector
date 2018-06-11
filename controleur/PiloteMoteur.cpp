#include "PiloteMoteur.h"

void PiloteMoteur::init(byte pinDir, byte pinPWM) {
	_pinDir = pinDir;
	_pinPWM = pinPWM;
	pinMode(_pinDir, OUTPUT);
	pinMode(_pinPWM, OUTPUT);
	setCommand(true, 0);
	setCurrentSpeed(0);
	update();

	_Kp = 255/280;
	_Ki = 1;
	_Kd = 1;

	PID aPid(&_input, &_output, &_setpoint, _Kp, _Ki, _Kd, DIRECT);
	_pid = &aPid;
	_pid->SetMode(AUTOMATIC);
	_pid->SetOutputLimits(0, 255);
	_pid->SetSampleTime(100);
}

void PiloteMoteur::setCommand(boolean directionRoue, long setpoint) {
	
	_setpoint = (double)setpoint;
	
	if(directionRoue) {
		digitalWrite(_pinDir, HIGH);
	}
	else {
		digitalWrite(_pinDir, LOW);
	}
}

void PiloteMoteur::setCurrentSpeed(long vitesseMesuree) {
	_input = (double)vitesseMesuree;
}

int PiloteMoteur::update() {

	_pid->Compute();

	analogWrite(_pinPWM, int(_output));

	return _output;
}
