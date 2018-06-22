#include "StateMove.h"

void StateMove::setData(int strafeDirection, byte rotationDirection) {
	_strafeDirection = strafeDirection;
	_rotationDirection = rotationDirection;
}

void StateMove::onStart() {
  _robot->_ecran.clear();
  _robot->_ecran.set(0, String("St:")+String(_strafeDirection));
  _robot->_ecran.set(1, String("Ro:")+String(_rotationDirection));
  _robot->_ecran.refresh();

	_robot->computeMove(_strafeDirection, _rotationDirection);

	_robot->_compteur1.readyToRead();
	_robot->_compteur2.readyToRead();

	_lastRefresh = millis();
}

State* StateMove::run() {

	if(millis() - _lastRefresh > REFRESH_DELAY) {

		_lastRefresh = millis();

		_robot->_compteur1.readyToRead();
		_robot->_compteur2.readyToRead();
	}

	SpeedSampleType* compteur1 = _robot->_compteur1.readIfAvailable();

	if (compteur1->newSampleAvailable) {

    _robot->_ecran.set(2, String("A:")+String(compteur1->speed2)+String(" B:")+String(compteur1->speed1));
    _robot->_ecran.refresh();

    _robot->_moteurA.setInput(compteur1->speed2);
    _robot->_moteurB.setInput(compteur1->speed1);

	  _robot->_moteurA.update();
	  _robot->_moteurB.update();
  }


  SpeedSampleType* compteur2 = _robot->_compteur2.readIfAvailable();
  
  if (compteur2->newSampleAvailable) {

		_robot->_ecran.set(3, String("D:")+String(compteur2->speed2)+String(" C:")+String(compteur2->speed1));
		_robot->_ecran.refresh();

    _robot->_moteurC.setInput(compteur2->speed1);
    _robot->_moteurD.setInput(compteur2->speed2);

	  _robot->_moteurC.update();
	  _robot->_moteurD.update();
  }

  return NULL;
}

void StateMove::onStop() {
}
