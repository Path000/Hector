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
  Serial1.setTimeout(1000);
  _compteur1.init(&Serial1);
  if(!Serial1) {
    Serial.print("Ooops, Serial1 port was not available !");
    while (1);
  }
  
  Serial2.begin(115200);
  Serial2.setTimeout(1000);
  _compteur2.init(&Serial2);
  if(!Serial2) {
    Serial.print("Ooops, Serial2 port was not available !");
    while (1);
  }
}


// STOP : MOVE:-1:0
// rotate : MOVE:-1:-1 OR MOVE:-1:1
// strafe forward : MOVE:0:0
// strafe left : MOVE:90:0
// all : MOVE:0:1


void Robot::computeMove(int strafeDirection, byte rotationDirection) {

  _ecran.clear();
  _ecran.set(0, String("Str.:")+String(strafeDirection));
  _ecran.set(1, String("Rot.:")+String(rotationDirection));
  _ecran.refresh();

  // cmd arg rotation : direction (int) -1 | 0 | 1
  double angularSpeed = ROTATION_COEF * rotationDirection * (DEMI_LARGEUR + DEMI_LONGUEUR);

  double wA = angularSpeed;
  double wB = -1*angularSpeed;
  double wC = -1*angularSpeed;
  double wD = angularSpeed;

  // cmd arg strafe : (int) angle en degrés -1 means no strafe
  if(strafeDirection >= 0) {
    double strafeX = STRAFE_COEF * cos(strafeDirection * PI / 180);
    double strafeY = STRAFE_COEF * sin(strafeDirection * PI / 180);
    wA = strafeY - strafeX + wA;
    wB = strafeY + strafeX + wB;
    wC = strafeY - strafeX + wC;
    wD = strafeY + strafeX + wD;
  }

  _moteurA.setCommand((wA>0)?true:false, long(wA));
  _moteurB.setCommand((wB>0)?true:false, long(wB));
  _moteurC.setCommand((wC>0)?true:false, long(wC));
  _moteurD.setCommand((wD>0)?true:false, long(wD));

  //_robot->getEcran()->set(2, String("A:")+String(long(wA))+String(" B:")+String(long(wB)));
  //_robot->getEcran()->set(3, String("D:")+String(long(wD))+String(" C:")+String(long(wC)));
  //_robot->getEcran()->refresh();

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
