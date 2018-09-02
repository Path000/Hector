// https://www.arduino.cc/en/Hacking/PinMappingSAM3X


/*
----- PIN MAPPING -----
0	RX0
1 	TX0 debug via USB
6	RPLidar MOTOCTL (PWM 25kHz)
3	Display RESET
13	LED_BUILTIN
18	TX1
19	RX1 Serial1 RPLidar UART
20	SDA
21	SCL i2c display (@ 0x3C)

*/

// TODO phase init odométrie :
// 1. détecter une ligne droire avec Douglas-Peucker
// 2. face à un point, recule de un tour de roue pour avoir avoir le ratio tick/m
// 3. tourne de 90° avec cette ligne droite pour avoir l'odométrie de ratation.
// 4. idem pour la translation.

#include <Arduino.h>
#include "Ecran.h"
#include "RPLidar.h"
#include "StateMachine.h"

Ecran ecran;
RPLidar lidar;
StateMachine stateMachine;

// Option grille d'occupation
//Surface à cartografier : 100  m2
// 1 m2 = 100 dm2

#define CELL_NUMBER 10000

struct Cell {
	int16_t x;
	int16_t y;
	uint8_t status;
};

Cell carte[CELL_NUMBER];

void setup() {

	for(uint32_t i=0; i<CELL_NUMBER; i++) {
		carte[i].x = 0;
		carte[i].y = 0;
		carte[i].status = 0;
	}


	Cell* ptr = (Cell*)malloc(CELL_NUMBER * sizeof(Cell));
	if(ptr == NULL) while(1); // Pas assez de mémoire

	free(ptr);


	Serial.begin(115200);

	Serial1.begin(115200);
	while(!Serial1);
	Serial1.setTimeout(3000);

	ecran.init();

	stateMachine.init();
}

void loop() {

	//Serial.println(Serial1.available());

	stateMachine.run();
}