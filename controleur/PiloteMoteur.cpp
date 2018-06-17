#include "PiloteMoteur.h"

void PiloteMoteur::init(byte pinDir, byte pinPWM) {
	_pinDir = pinDir;
	_pinPWM = pinPWM;
	pinMode(_pinDir, OUTPUT);
	pinMode(_pinPWM, OUTPUT);
	setCommand(true, 0);
	setCurrentSpeed(0);
	//update();
	stop();

	_Kp = 255/MAX_SPEED;
	_Ki = 1;
	_Kd = 1;

/*
	PID aPid(&_input, &_output, &_setpoint, _Kp, _Ki, _Kd, DIRECT);
	_pid = &aPid;
	_pid->SetMode(AUTOMATIC);
	_pid->SetOutputLimits(0, 255);
	_pid->SetSampleTime(100);
*/
}

void PiloteMoteur::setCommand(boolean directionRoue, double setpoint) {
	
	_setpoint = setpoint;
	
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

boolean PiloteMoteur::update() {

	_output = _Kp * _setpoint;

	boolean computed = true; //_pid->Compute();
	if(computed) {
	Serial.println(constrain((int)_output, 0, 255));

		analogWrite(_pinPWM, constrain((int)_output, 0, 255));
	}

	return computed;
}

void PiloteMoteur::stop() {
	analogWrite(_pinPWM, 0);
}
