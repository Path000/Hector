#include "PiloteMoteur.h"

void PiloteMoteur::init(byte pinDir, byte pinPWM, String whoami) {
	_pinDir = pinDir;
	_pinPWM = pinPWM;
	_whoami = whoami;
	pinMode(_pinDir, OUTPUT);
	pinMode(_pinPWM, OUTPUT);

	_Kp = 0.0;
	_Ki = 0.0;
	_Kd = 0.0;
	
	reset();

	//PID aPid(&_input, &_output, &_setpoint, _Kp, _Ki, _Kd, DIRECT);
	//_pid = &aPid;
	//_pid->SetOutputLimits(0.0, 255.0);
	//_pid->SetSampleTime(100);
	//_pid->SetMode(AUTOMATIC);
}

void PiloteMoteur::reset() {

	setCommand(0);
	setSpeedSample(0);
	_outputSum = 0.0;
	_lastInput = 0;
	
	stop();
}

void PiloteMoteur::setCommand(int command) {
	
	if(command >= 0) {
		digitalWrite(_pinDir, HIGH);
	}
	else {
		digitalWrite(_pinDir, LOW);
	}

	_setpoint = abs(command);
	
	Serial.print(_whoami);
	Serial.print(": cmd:");
	Serial.println(_setpoint);
}

void PiloteMoteur::setSpeedSample(unsigned int vitesseMesuree) {
	_input = vitesseMesuree;
}

void PiloteMoteur::update() {

	int error = _setpoint - _input;
	int dInput = _input - _lastInput;
	_lastInput = _input;

	_outputSum += _Ki * (double)error;

	if(_outputSum < 0.0) _outputSum = 0.0;
	if(_outputSum > 255.0) _outputSum = 255.0;

	double outputAsDouble = (_Kp * (double)error) + _outputSum - (_Kd * (double)dInput);

	//byte output = constrain((int)outputAsDouble, 0, 255);
	byte output = constrain((int)_setpoint, 0, 255);

	analogWrite(_pinPWM, output);
}

void PiloteMoteur::stop() {
	analogWrite(_pinPWM, 0);
}

void PiloteMoteur::setPID(float Kp, float Ki, float Kd) {
	_Kp = Kp;
	_Ki = Ki;
	_Kd = Kd;
	reset();
}
