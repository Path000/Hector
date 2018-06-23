#ifndef PiloteMoteur_h
#define PiloteMoteur_h

#include <Arduino.h>

class PiloteMoteur {
	public :
		void init(byte pinDir, byte pinPWM, String whoami);
		void setCommand(int command);
		void setSpeedSample(unsigned int vitesseMesuree);
		void update();
		void stop();
	private :
		byte _pinDir;
		byte _pinPWM;
		unsigned int _setpoint;
		unsigned int _input;
		float _Kp;
		float _Ki;
		float _Kd;
		String _whoami;
		unsigned int _lastInput;
		double _outputSum;
};

#endif
