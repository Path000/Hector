/*
----- PIN MAPPING -----

6	RPLidar MOTOCTL (PWM 25kHz)
3	Display RESET
13	LED_BUILTIN
18	TX1
19	RX1 Serial1 RPLidar UART
20	SDA
21	SCL i2c display (@ 0x3C)

*/

#include "Ecran.h"
#include "RPLidar.h"
#include "StateMachine.h"

Ecran ecran;
RPLidar lidar;
StateMachine stateMachine;

void setup() {

  // Serial : 0 (RX) and 1 (TX) => Debug USB
  Serial.begin(115200);

  // Serial 1: 19 (RX) and 18 (TX) => RP Lidar
  Serial1.begin(115200);
  while(!Serial1);
  Serial1.setTimeout(3000);

  ecran.init();

  stateMachine.init();
}

void loop() {

	Serial.println(Serial1.available());

	stateMachine.run();
}