#include "StateMove.h"

void StateMove::setStrafe(int direction) {
	_strafeDirection = direction;
}

void StateMove::setRotation(byte direction) {
	_rotationDirection = direction;
}

void StateMove::onStart() {
	_robot->getEcran()->clear();
	_robot->getEcran()->set(0, String("Str.:")+String(_strafeDirection));
	_robot->getEcran()->set(1, String("Rot.:")+String(_rotationDirection));
	_robot->getEcran()->refresh();

	_robot->getCompteur1()->resetSequence();
	_robot->getCompteur2()->resetSequence();

	// cmd arg rotation : direction (int) -1 | 0 | 1
	double angularSpeed = ROTATION_COEF * _rotationDirection * (DEMI_LARGEUR + DEMI_LONGUEUR);

	double wA = angularSpeed;
	double wB = -1*angularSpeed;
	double wC = -1*angularSpeed;
	double wD = angularSpeed;

	// cmd arg strafe : (int) angle en degrés -1 means no strafe
	if(_strafeDirection >= 0) {
		double strafeX = STRAFE_COEF * cos(_strafeDirection * PI / 180);
		double strafeY = STRAFE_COEF * sin(_strafeDirection * PI / 180);
		wA = strafeY - strafeX + wA;
		wB = strafeY + strafeX + wB;
		wC = strafeY - strafeX + wC;
		wD = strafeY + strafeX + wD;
	}

	_robot->getMoteurA()->setCommand((wA>0)?true:false, long(wA));
	_robot->getMoteurB()->setCommand((wB>0)?true:false, long(wB));
	_robot->getMoteurC()->setCommand((wC>0)?true:false, long(wC));
	_robot->getMoteurD()->setCommand((wD>0)?true:false, long(wD));

	_robot->getEcran()->set(2, String("A:")+String(long(wA))+String(" B:")+String(long(wB)));
	_robot->getEcran()->set(3, String("D:")+String(long(wD))+String(" C:")+String(long(wC)));
	_robot->getEcran()->refresh();

}

State* StateMove::run() {

	if (_robot->getCompteur1()->read()) {

    if (_robot->getCompteur1()->hasLostSample()) {
      _robot->getEcran()->set(0, "SAMPLE LOST 1");
      _robot->getEcran()->refresh();
      //TODO : lisser compteur1.getNumberOfSampleLost()
    }

    long speedA = _robot->getCompteur1()->getSpeed2();
    long speedB = _robot->getCompteur1()->getSpeed1();

    _robot->getMoteurA()->update(speedA);
    _robot->getMoteurB()->update(speedB);

    _robot->getCompteur1()->ready();
  }

  if (_robot->getCompteur2()->read()) {

    if (_robot->getCompteur2()->hasLostSample()) {
      _robot->getEcran()->set(1, "SAMPLE LOST 2");
      _robot->getEcran()->refresh();
      //TODO : lisser compteur2.getNumberOfSampleLost()
    }

    long speedC = _robot->getCompteur2()->getSpeed1();
    long speedD = _robot->getCompteur2()->getSpeed2();

    _robot->getMoteurC()->update(speedC);
    _robot->getMoteurD()->update(speedD);

    _robot->getCompteur2()->ready();
  }

	return NULL;
}

void StateMove::onStop() {
	_robot->getEcran()->clear();
}