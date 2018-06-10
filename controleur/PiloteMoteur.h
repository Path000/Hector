#ifndef PiloteMoteur_h
#define PiloteMoteur_h

#include <Arduino.h>
#include "PID.h"

class PiloteMoteur {
	public :
		void init(byte pinDir, byte pinPWM);
		void setCommand(boolean directionRoue, long setpoint);
		void update(long vitesseMesuree);
		void stop();
	private :
		byte _pinDir;
		byte _pinPWM;
		double _setpoint;
		double _input;
		double _output;
		double _Kp;
		double _Ki;
		double _Kd;
		PID* _pid;
};

#endif
