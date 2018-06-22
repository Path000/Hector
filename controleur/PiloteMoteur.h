#ifndef PiloteMoteur_h
#define PiloteMoteur_h

#include <Arduino.h>
#include "PID.h"

#define COMPUTE_PERIOD 100 // ms

class PiloteMoteur {
	public :
		void init(byte pinDir, byte pinPWM, String whoami);
		void setSetPoint(boolean directionRoue, unsigned int setpoint);
		void setInput(unsigned int vitesseMesuree);
		boolean update();
		void stop();
	private :
		byte _pinDir;
		byte _pinPWM;
		unsigned int _setpoint;
		unsigned int _input;
		float _Kp;
		float _Ki;
		float _Kd;
		//PID* _pid;
		String _whoami;
		unsigned long _lastComputedTime;
		unsigned int _lastInput;
		double _outputSum;
};

#endif
