#ifndef Robot_h
#define Robot_h

#include "Compteur.h"
#include "Ecran.h"
#include "BNOSensor.h"
#include "Command.h"
#include "PiloteMoteur.h"

// TODO finir câblage

#define PIN_DIR_A 0
#define PIN_PWM_A 0
#define PIN_DIR_B 0
#define PIN_PWM_B 0
#define PIN_DIR_C 0
#define PIN_PWM_C 0
#define PIN_DIR_D 0
#define PIN_PWM_D 0

#define DISTANCE_A 13 //cm
#define DISTANCE_B 13 //cm

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
