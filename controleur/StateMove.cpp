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

	//_robot->getEcran()->set(2, String("A:")+String(long(wA))+String(" B:")+String(long(wB)));
	//_robot->getEcran()->set(3, String("D:")+String(long(wD))+String(" C:")+String(long(wC)));
	//_robot->getEcran()->refresh();

}

State* StateMove::run() {

	SpeedSampleType compteur1 = _robot->getCompteur1()->readIfAvailable();
	if (compteur1.newSampleAvailable) {

    if (compteur1.deltaSequence > 0) {
      //_robot->getEcran()->set(0, "SAMPLE LOST 1");
      //_robot->getEcran()->refresh();
      //TODO : lisser compteur1.getNumberOfSampleLost()
    }

    _robot->getMoteurA()->update(compteur1.speed2);
    _robot->getMoteurB()->update(compteur1.speed1);

    _robot->getEcran()->set(2, String("A:")+String(compteur1.speed2)+String(" B:")+String(compteur1.speed1));
    _robot->getEcran()->refresh();
  }

  SpeedSampleType compteur2 = _robot->getCompteur2()->readIfAvailable();
  if (compteur2.newSampleAvailable) {

    if (compteur2.deltaSequence > 0) {
      //_robot->getEcran()->set(1, "SAMPLE LOST 2");
      //_robot->getEcran()->refresh();
      //TODO : lisser compteur2.getNumberOfSampleLost()
    }

    _robot->getMoteurC()->update(compteur2.speed1);
    _robot->getMoteurD()->update(compteur2.speed2);

		_robot->getEcran()->set(3, String("D:")+String(compteur2.speed2)+String(" C:")+String(compteur2.speed1));
		_robot->getEcran()->refresh();
  }
/*
    _robot->getMoteurA()->update(10);
    _robot->getMoteurB()->update(10);
    _robot->getMoteurC()->update(10);
    _robot->getMoteurD()->update(10);
*/
	return NULL;
}

void StateMove::onStop() {
	_robot->getEcran()->clear();
}