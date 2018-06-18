#include "PiloteMoteur.h"

void PiloteMoteur::init(byte pinDir, byte pinPWM, String whoami) {
	_pinDir = pinDir;
	_pinPWM = pinPWM;
	_whoami = whoami;
	pinMode(_pinDir, OUTPUT);
	pinMode(_pinPWM, OUTPUT);
	setCommand(true, 0);
	setCurrentSpeed(0);
	
	//stop();

	_Kp = 0.8;
	_Ki = 0.2;
	_Kd = 0.2;

	PID aPid(&_input, &_output, &_setpoint, _Kp, _Ki, _Kd, DIRECT);
	_pid = &aPid;
	_pid->SetOutputLimits(0.0, 255.0);
	_pid->SetSampleTime(100);
	_pid->SetMode(AUTOMATIC);

	update();
}

void PiloteMoteur::setCommand(boolean directionRoue, double setpoint) {
	
	_setpoint = setpoint;
	
	if(directionRoue) {
		digitalWrite(_pinDir, HIGH);
	}
	else {
		digitalWrite(_pinDir, LOW);
	}

	Serial.print(_whoami);
	Serial.print(":set:");
	Serial.println(_setpoint, 2);
}

void PiloteMoteur::setCurrentSpeed(long vitesseMesuree) {
	_input = (double)vitesseMesuree;
}

boolean PiloteMoteur::update() {

	_output = 20.0;

	boolean computed = _pid->Compute();
	if(computed) {
		Serial.print(_whoami);
		Serial.print(":upd:");
		Serial.println(_output);
		analogWrite(_pinPWM, constrain((int)_output, 0, 255));
	}

	return computed;
}

void PiloteMoteur::stop() {
	analogWrite(_pinPWM, 0);
}
