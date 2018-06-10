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
#define PIN_DIR_C 3
#define PIN_PWM_C 2
#define PIN_DIR_D 5
#define PIN_PWM_D 4

class Robot {
	public:
		void init();
		Compteur* getCompteur1();
		Compteur* getCompteur2();
		Ecran* getEcran();
		BNOSensor* getSensor();
		Command* getCommand();
		PiloteMoteur* getMoteurA();
		PiloteMoteur* getMoteurB();
		PiloteMoteur* getMoteurC();
		PiloteMoteur* getMoteurD();
		void stopMoteurs();
	private:
		BNOSensor _sensor;
		Ecran _ecran;
		Compteur _compteur1; // Compteur A et B
		Compteur _compteur2; // Compteur C et D
		Command _command;
		PiloteMoteur _moteurA;
		PiloteMoteur _moteurB;
		PiloteMoteur _moteurC;
		PiloteMoteur _moteurD;
};


#endif