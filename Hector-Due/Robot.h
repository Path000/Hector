#ifndef Robot_h
#define Robot_h

#include "Ecran.h"
#include "RPLidar.h"

class Robot {
	public:
		void init();

		Ecran _ecran;
		RPLidar _lidar;
};


void Robot::init() {

  // Serial : 0 (RX) and 1 (TX) => Debug USB
  Serial.begin(115200);

  // Serial 1: 19 (RX) and 18 (TX) => RP Lidar
  Serial1.begin(115200);
  while(!Serial1);
  Serial1.setTimeout(3000);

  _ecran.init();
}

#endif