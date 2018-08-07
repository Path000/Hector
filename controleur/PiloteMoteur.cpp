#include "PiloteMoteur.h"

void PiloteMoteur::init(byte pinDir, byte pinPWM, String whoami) {
	_pinDir = pinDir;
	_pinPWM = pinPWM;
	_whoami = whoami;
	pinMode(_pinDir, OUTPUT);
	pinMode(_pinPWM, OUTPUT);

	setCommand(0);
	
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
	
	//Serial.print(_whoami);
	//Serial.print(": cmd:");
	//Serial.println(_setpoint);
}

// TODO mesuer la vitesse à 255 au démarrage du robot

void PiloteMoteur::update(unsigned int vitesseMesuree, unsigned int duration) {

	Serial.print(vitesseMesuree, DEC);
	Serial.print(",");
	Serial.println(_setpoint, DEC);

	float output = (float)_setpoint * 255.0 / 280.0; // PWM 100% -> 280 fronts/100ms

	if( duration < 3000) { // On accélère sur 3s de 0 à output

		output = output * duration / 3000;
	}
	else { // On gère une correction proportionnelle basée sur l'erreur

		float errorRatio = 0;
		if(_setpoint > 0) {
			errorRatio = (_setpoint - vitesseMesuree) / _setpoint * 255.0 / 280.0;
		}
		
		output = output + (output * errorRatio);
	}
	
	byte command = constrain((int)output, 0, 255);

	analogWrite(_pinPWM, command);
}

void PiloteMoteur::stop() {
	analogWrite(_pinPWM, 0);
}