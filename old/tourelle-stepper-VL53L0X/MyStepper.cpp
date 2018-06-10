#include "MyStepper.h"

MyStepper::MyStepper() {
  stepper = AccelStepper(1, PIN_STEPPER_STEP, PIN_STEPPER_DIR);
}

void MyStepper::init() {
  stepper.setMaxSpeed(5000);
  stepper.setAcceleration(5000);
}

void MyStepper::move(int stepSize) {
  stepper.move(stepSize);
  stepper.runToPosition();
}

