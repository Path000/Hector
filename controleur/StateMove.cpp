#include "StateMove.h"

void StateMove::setData(String strafeDirection, String rotationDirection) {

	_robot->computeMove(strafeDirection.toInt(), rotationDirection.toInt());

	_robot->_ecran.clear();
  _robot->_ecran.set(0, String("St:")+strafeDirection);
  _robot->_ecran.set(1, String("Ro:")+rotationDirection);
  _robot->_ecran.refresh();

}

void StateMove::onStart() {

	_robot->_compteur1.readyToRead();
	_robot->_compteur2.readyToRead();

	_lastRefresh = millis();
}

State* StateMove::run() {

	unsigned long now = millis();

	if((now - _lastRefresh) > UPDATE_PID_PERIOD) {

		_lastRefresh = now;

		_robot->_compteur1.readyToRead();
		_robot->_compteur2.readyToRead();
	}

	SpeedSampleType* compteur1 = _robot->_compteur1.readIfAvailable();

	if (compteur1->newSampleAvailable) {

    _robot->_ecran.set(2, String("A:")+String(compteur1->speed2)+String(" B:")+String(compteur1->speed1));
    _robot->_ecran.refresh();

    _robot->_moteurA.setSpeedSample(compteur1->speed2);
    _robot->_moteurA.update();

    _robot->_moteurB.setSpeedSample(compteur1->speed1);
    _robot->_moteurB.update();
  }

  SpeedSampleType* compteur2 = _robot->_compteur2.readIfAvailable();
  
  if (compteur2->newSampleAvailable) {

		_robot->_ecran.set(3, String("D:")+String(compteur2->speed2)+String(" C:")+String(compteur2->speed1));
		_robot->_ecran.refresh();

    _robot->_moteurC.setSpeedSample(compteur2->speed1);
    _robot->_moteurC.update();

    _robot->_moteurD.setSpeedSample(compteur2->speed2);
    _robot->_moteurD.update();
  }

  return NULL;
}
