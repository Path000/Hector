#ifndef Robot_h
#define Robot_h

#include "Compteur.h"
#include "Ecran.h"
#include "BNOSensor.h"
#include "Command.h"
#include "PiloteMoteur.h"

#define PIN_DIR_A 9
#define PIN_PWM_A 8
#define PIN_DIR_B 11
#define PIN_PWM_B 10
#define PIN_DIR_C 5
#define PIN_PWM_C 4
#define PIN_DIR_D 7
#define PIN_PWM_D 6

// PWM 100% -> 280 fronts/100ms

#define DEMI_LARGEUR 0.13 //m
#define DEMI_LONGUEUR 0.13 //m 

#define STRAFE_COEF 20.0
#define ROTATION_COEF 90.0

class Robot {
	public:
		void init();
		void stopMotors();
		void computeMove(int strafeDirection, int rotationDirection);

		BNOSensor _sensor;
		Ecran _ecran;
		Compteur _compteur1; // Compteur A et B
		Compteur _compteur2; // Compteur C et D
		Command _command;
		PiloteMoteur _moteurA;
		PiloteMoteur _moteurB;
		PiloteMoteur _moteurC;
		PiloteMoteur _moteurD;
	private:
};


#endif
