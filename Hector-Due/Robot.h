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

#endif