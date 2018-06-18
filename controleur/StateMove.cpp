#include "StateMove.h"

void StateMove::setData(int strafeDirection, byte rotationDirection) {
	_strafeDirection = strafeDirection;
	_rotationDirection = rotationDirection;
}

void StateMove::onStart() {
  _robot->getEcran()->clear();
  _robot->getEcran()->set(0, String("St:")+String(_strafeDirection));
  _robot->getEcran()->set(1, String("Ro:")+String(_rotationDirection));
  _robot->getEcran()->refresh();

	_robot->computeMove(_strafeDirection, _rotationDirection);

	_robot->getCompteur1()->readyToRead();
	_robot->getCompteur2()->readyToRead();

	_lastRefresh = millis();
}

State* StateMove::run() {

	if(millis() - _lastRefresh > REFRESH_DELAY) {

		_lastRefresh = millis();

		_robot->getCompteur1()->readyToRead();
		_robot->getCompteur2()->readyToRead();
	}

	SpeedSampleType* compteur1 = _robot->getCompteur1()->readIfAvailable();

	if (compteur1->newSampleAvailable) {

    _robot->getEcran()->set(2, String("A:")+String(compteur1->speed2)+String(" B:")+String(compteur1->speed1));
    _robot->getEcran()->refresh();

    _robot->getMoteurA()->setCurrentSpeed(compteur1->speed2);
    _robot->getMoteurB()->setCurrentSpeed(compteur1->speed1);

	  _robot->getMoteurA()->update();
	  _robot->getMoteurB()->update();
  }


  SpeedSampleType* compteur2 = _robot->getCompteur2()->readIfAvailable();
  
  if (compteur2->newSampleAvailable) {

		_robot->getEcran()->set(3, String("D:")+String(compteur2->speed2)+String(" C:")+String(compteur2->speed1));
		_robot->getEcran()->refresh();

    _robot->getMoteurC()->setCurrentSpeed(compteur2->speed1);
    _robot->getMoteurD()->setCurrentSpeed(compteur2->speed2);

	  _robot->getMoteurC()->update();
	  _robot->getMoteurD()->update();
  }

  return NULL;
}

void StateMove::onStop() {
}
