#include "PiloteMoteur.h"

void PiloteMoteur::init(byte pinDir, byte pinPWM) {
	_pinDir = pinDir;
	_pinPWM = pinPWM;
	pinMode(_pinDir, OUTPUT);
	pinMode(_pinPWM, OUTPUT);
	//setCommand(true, 0);
	//update(0);
	stop();

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

void PiloteMoteur::update(long vitesseMesuree) {

	_input = (double)vitesseMesuree;

	_pid->Compute();

	//Serial.println(_output);

	analogWrite(_pinPWM, int(_output));
}

void PiloteMoteur::stop() {
	analogWrite(_pinPWM, 0);
}