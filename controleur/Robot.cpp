#include "Robot.h"

void Robot::init() {

  Serial.begin(115200); // RaspberryPI
  Serial.setTimeout(500);
  while (!Serial); // wait for serial port to connect. Needed for native USB
  _command.init(&Serial);

  _moteurA.init(PIN_DIR_A, PIN_PWM_A);
  _moteurB.init(PIN_DIR_B, PIN_PWM_B);
  _moteurC.init(PIN_DIR_C, PIN_PWM_C);
  _moteurD.init(PIN_DIR_D, PIN_PWM_D);

  _sensor.init();
  
  _ecran.init();
  
  Serial1.begin(115200);
  Serial1.setTimeout(500);
  _compteur1.init(&Serial1);
  if(!Serial1) {
    Serial.print("Ooops, Serial1 port was not available !");
    while (1);
  }
  
  Serial2.begin(115200);
  Serial2.setTimeout(500);
  _compteur2.init(&Serial2);
  if(!Serial2) {
    Serial.print("Ooops, Serial2 port was not available !");
    while (1);
  }
}

void Robot::stopMoteurs() {
  _moteurA.stop();
  _moteurB.stop();
  _moteurC.stop();
  _moteurD.stop();
}

Command* Robot::getCommand() {
  return &_command;
}

Compteur* Robot::getCompteur1() {
  return &_compteur1;
}

Compteur* Robot::getCompteur2() {
	return &_compteur2;
}

Ecran* Robot::getEcran() {
	return &_ecran;
}

BNOSensor* Robot::getSensor() {
	return &_sensor;
}

PiloteMoteur* Robot::getMoteurA() {
  return &_moteurA;
}

PiloteMoteur* Robot::getMoteurB() {
  return &_moteurB;
}

PiloteMoteur* Robot::getMoteurC() {
  return &_moteurC;
}

PiloteMoteur* Robot::getMoteurD() {
  return &_moteurD;
}
