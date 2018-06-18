#ifndef PiloteMoteur_h
#define PiloteMoteur_h

#define MAX_SPEED 280 // PWM 100% -> 280 fronts/100ms

#include <Arduino.h>
#include "PID.h"

class PiloteMoteur {
	public :
		void init(byte pinDir, byte pinPWM, String whoami);
		void setCommand(boolean directionRoue, double setpoint);
		void setCurrentSpeed(long vitesseMesuree);
		boolean update();
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
		String _whoami;
};

#endif
