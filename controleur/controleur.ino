#include <Arduino.h>


#include "Robot.h"
#include "StateMachine.h"

Robot robot;
StateMachine stateMachine;

void setup() {

  robot.init();

  stateMachine.init(&robot);
}

void loop() {

  stateMachine.run();
}
