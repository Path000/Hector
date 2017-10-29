#ifndef MyStepper_h
#define MyStepper_h

#include <AccelStepper.h>
// https://www.pjrc.com/teensy/td_libs_AccelStepper.html
// Merci Ashira

#define PIN_STEPPER_STEP 12
#define PIN_STEPPER_DIR 11

class MyStepper {
  
  private:
    AccelStepper stepper;
 
  public:
  
    MyStepper();
    void init();
    void move(int stepSize);
};

#endif
