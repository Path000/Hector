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


#include "Robot.h"
#include "StateMachine.h"

Robot robot;
StateMachine stateMachine;

void setup() {

	robot.init();
}

void loop() {

	Serial.println(Serial1.available());

	stateMachine.run();
}