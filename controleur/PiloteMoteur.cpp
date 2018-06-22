#include "PiloteMoteur.h"

void PiloteMoteur::init(byte pinDir, byte pinPWM, String whoami) {
	_pinDir = pinDir;
	_pinPWM = pinPWM;
	_whoami = whoami;
	pinMode(_pinDir, OUTPUT);
	pinMode(_pinPWM, OUTPUT);

	_Kp = 0.8;
	_Ki = 0.2;
	_Kd = 0.2;
	
	setSetPoint(true, 0);
	setInput(0);
	_lastComputedTime = millis();
	_outputSum = 0.0;
	stop();

	//PID aPid(&_input, &_output, &_setpoint, _Kp, _Ki, _Kd, DIRECT);
	//_pid = &aPid;
	//_pid->SetOutputLimits(0.0, 255.0);
	//_pid->SetSampleTime(100);
	//_pid->SetMode(AUTOMATIC);
}

void PiloteMoteur::setSetPoint(boolean directionRoue, unsigned int setpoint) {
	
	_setpoint = setpoint;
	
	if(directionRoue) {
		digitalWrite(_pinDir, HIGH);
	}
	else {
		digitalWrite(_pinDir, LOW);
	}

	Serial.print(_whoami);
	Serial.print(" set:");
	Serial.println(_setpoint, 2);
}

void PiloteMoteur::setInput(unsigned int vitesseMesuree) {
	_input = vitesseMesuree;
}

boolean PiloteMoteur::update() {

	Serial.print(_whoami);
	
	unsigned long now = millis();

	if(now - _lastComputedTime > COMPUTE_PERIOD) {

		int error = _setpoint - _input;
		int dInput = _input - _lastInput;

		_outputSum += _Ki * (double)error;

		if(_outputSum < 0.0) _outputSum = 0.0;
		if(_outputSum > 255.0) _outputSum = 255.0;

		double outputAsDouble = _Kp * (double)error + _outputSum - (_Kd * (double)dInput);

		int output = constrain((int)outputAsDouble, 0, 255);

		_lastInput = _input;
		_lastComputedTime = now;

		analogWrite(_pinPWM, output);

		Serial.print(" out:");
		Serial.print(output);

		return true;
	}

	return false;
}

void PiloteMoteur::stop() {
	analogWrite(_pinPWM, 0);
}
