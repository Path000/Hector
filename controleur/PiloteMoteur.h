#ifndef PiloteMoteur_h
#define PiloteMoteur_h

#include <Arduino.h>

class PiloteMoteur {
	public :
		void init(byte pinDir, byte pinPWM, String whoami);
		void setCommand(int command);
		void update(unsigned int vitesseMesuree, unsigned int duration);
		void stop();
	private :
		byte _pinDir;
		byte _pinPWM;
		unsigned int _setpoint;
		String _whoami;
};

#endif
