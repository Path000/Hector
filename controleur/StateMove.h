#ifndef StateMove_h
#define StateMove_h

#include "State.h"

#define DEMI_LARGEUR 0.13 //m
#define DEMI_LONGUEUR 0.13 //m


// PWM 100% -> 280 fronts/100ms

#define STRAFE_COEF 100
#define ROTATION_COEF 10

class StateMove : public State {
	public:
		void onStart();
		State* run();
		void onStop();
		void setStrafe(int direction);
		void setRotation(byte direction);
	private:
		int _strafeDirection;
		byte _rotationDirection;
};


#endif